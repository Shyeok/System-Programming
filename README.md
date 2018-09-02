# System-Programming
## SIC/XE Machine(Project 1 ~ 3)
### Project 1
+ 프로젝트 설명

이 프로그램은 어셈블러, 링크, 로더들을 실행하게 될 셸(shell)과 컴파일을 통해서 만들어진 object코드가 적재되고 실행될 메모리공간과 mnemonic(add, COMP, FLOAT, etc ..)을 opcode값으로 변환하는 OPCODE 테이블과 관련 명령어들을 구현하는 프로그램입니다.

+ 프로그램 흐름도
<img src="https://user-images.githubusercontent.com/38277490/44953398-ed72ff00-aecf-11e8-9a4f-1cb2afd406f6.png" width = "50%">

### Project 2
+ 프로젝트 설명

SIC/XE의 assembly program source 파일을 입력 받아서 object파일을 생성하고, 어셈블리 과정 중 생성된 symbol table과 결과물인 object 파일을 볼 수 있는 기능을 제공하는 프로그램을 작성한다.

### Project 3
+ 프로젝트 설명

프로젝트 #2 에서 구현된 assemble 명령을 통해서 생성된 object 파일을 link시켜 메모리에 올리는 일을 수행한다. Breakpoint 설정이 가능하다.
