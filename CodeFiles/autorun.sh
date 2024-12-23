#!/bin/bash
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
PURPLE='\033[0;36m'
BLUE='\033[1;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color
BOLD='\033[1m'


cd ~/xsm_expl/CodeFiles/
echo -e "${BLUE}Running YACC....${NC}"
yacc -d extree.y
echo -e "${BLUE}Running LEX....${NC}"
lex extree.l
echo -e "${BLUE}Compiling C files....${NC}"
gcc y.tab.c lex.yy.c tree.c generator.c

echo -e "${YELLOW}Compiling expl....${NC}"

./a.out || echo -e "${RED}${BOLD}Code generation failed !"

echo -e "${GREEN}${BOLD}Code generated${NC}"

echo -e "${YELLOW}Press any key to interrupt...${NC}"
stty -echo
read -t 2 -n 1 && stty echo && echo -e "${RED}${BOLD}Skipping Simulation...${NC}" && exit 0
stty echo
cd ~/xsm_expl
echo -e "${PURPLE}${BOLD}Simulating on XSM....${NC}"
./xsm -l library.lib -e CodeFiles/output.xsm
exit 0



