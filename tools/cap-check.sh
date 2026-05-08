#!/bin/bash

RED='\033[38;2;255;0;0m'
GREEN='\033[38;2;0;255;0m'
YELLOW='\033[38;2;255;255;0m'
BLUE='\033[38;2;0;0;255m'
NC='\033[0m'


required_groups=()

echo -e "${BLUE}DRM/KMS availability (/dev/dri/card*):${NC}"
drm_cards=$(ls /dev/dri/card* 2>/dev/null)
if [ -z "$drm_cards" ]; then
	echo -e "\t${RED}No DRM cards found${NC}"
else
	for card in $drm_cards; do
		group=$(stat -c "%G" "$card")
		perms=$(stat -c "%A %U:%G" "$card")
		echo -en "\t$card ($perms):\t"
		
		if [ -r "$card" ] && [ -w "$card" ]; then
			echo -e -n "[${GREEN}OK${NC}]     \t"
		else
			echo -e -n "[${RED}NO ACCESS${NC}]\t"
			required_groups+=("$group")
		fi

		card_id=$(basename "$card" | sed 's/card//')
		debug_path="/sys/kernel/debug/dri/$card_id/clients"
		if [ -f "$debug_path" ] && grep -q "y" "$debug_path" 2>/dev/null; then
			echo -e "[${YELLOW}MASTER ACTIVE${NC}]"
		elif lsof "$card" &>/dev/null; then
			echo -e "[${YELLOW}LIKELY MASTER ACTIVE${NC}]"
		else
			echo -e "[${GREEN}AVAILABLE${NC}]"
		fi
	done
fi

echo -e "\n${BLUE}Input device availablity (/dev/input/*):${NC}"
input_devs=$(ls /dev/input/event* 2>/dev/null)
if [ -z "$input_devs" ]; then
	echo -e "\t${RED}No input devices found${NC}"
else
	for dev in $input_devs; do
		group=$(stat -c "%G" "$dev")
		perms=$(stat -c "%A %U:%G" "$dev")

		echo -en "\t$dev ($perms):\t"
		if [ -r "$dev" ] && [ -w "$dev" ]; then
			echo -e "[${GREEN}OK${NC}]"
		else
			echo -e "[${RED}NO ACCESS${NC}]"
			required_groups+=("$group")
		fi
	done
fi

echo -e "\n${BLUE}Tools:${NC}"
for tool in g++ objcopy make expect python3; do
	echo -en "\t$tool: "
	if command -v "$tool" &> /dev/null; then
		echo -e "${GREEN}FOUND${NC} ($(($tool --version 2>/dev/null || $tool -v 2>/dev/null || echo "${YELLOW}version unknown${NC}") | head -n 1 ))"
	else
		echo -e "${RED}MISSING${NC}"
	fi
done

echo -e "\n${BLUE}Compiler:${NC}"
gcc_version=$(g++ -dumpversion 2>/dev/null | cut -d. -f1)

if [[ -z "$gcc_version" ]]; then
	echo -e "\t${RED}GCC not found${NC}"
elif [[ "$gcc_version" -lt 16 ]]; then
	echo -e "\t${RED}GCC version is $gcc_version. Needs 16${NC}"
else
	echo -e "\tGCC ${GREEN}FOUND${NC}"
	tmp_src=$(mktemp test_meta_XXXXXX.cpp)
	tmp_bin=$(mktemp test_meta_XXXXXX.out)
	cat << EOF > "$tmp_src"
#include <meta>
#include <iostream>
int main() {
	constexpr std::meta::info i = ^^int;
	if (std::meta::is_type(i)) {
		std::cout << "Reflection supported" << std::endl;
		return 0;
	}
	return 1;
}
EOF
	if g++ "$tmp_src" -o "$tmp_bin" -freflection -std=c++26 &>/dev/null && ./"$tmp_bin" &>/dev/null; then
		echo -e "\tGCC $gcc_version ${GREEN}SUPPORTS${NC} <meta> reflection"
	else
		echo -e "\t${RED}GCC 16 present but doesn't support <meta>${NC}"
	fi
	rm -f "$tmp_src" "$tmp_bin"
fi

echo -e "\n${BLUE}Hardcoded objcopy output format:${NC}"
current_arch=$(uname -m)
default_flag="elf64-x86-64"

declare -A arch_map
arch_map["x86_64"]="elf64-x86-64"
arch_map["aarch64"]="elf64-littleaarch64"
arch_map["i386"]="elf32-i386"
arch_map["i686"]="elf32-i386"
arch_map["armv7l"]="elf32-littlearm"
arch_map["riscv64"]="elf64-littleriscv"
arch_map["ppc64le"]="elf64-powerpcle"

CORRECT_O="${arch_map[$current_arch]}"

echo -en "\tArchitecture is $current_arch: "
if [[ "$current_arch" == "x86_64" ]]; then
	echo -e "[${GREEN}OK${NC}]"
elif [[ -n "$CORRECT_O" ]]; then
	echo -e "\t${YELLOW}WRONG FLAG${NC}"
	echo -e "\tFix: ${BLUE}sed -i 's/-O $default_flag/-O $CORRECT_O/g' Makefile${NC}"
else
	echo -e "\t${RED}WRONG FLAG${NC}"
	echo -e "\t${RED}Correct -O flag unknown${NC}"
fi

echo -e "\n${BLUE}Permissions:${NC}"

unique_groups=$(echo "${required_groups[@]}" | tr ' ' '\n' | sort -u | tr '\n' ',' | sed 's/,$//')
if [ -n "$unique_groups" ]; then
	echo -e "\t[${RED}]INSUFFICIENT PERMISSIONS${NC}]"
	echo -e "\tFix: ${BLUE}sudo usermod -aG $unique_groups $USER${NC}"
	echo -e "\t${YELLOW}Note:${NC} You will need to log out and back in for changes to take effect"
else
	echo -e "\tAll [${GREEN}OK${NC}]"
fi

