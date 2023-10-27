# TCP_DB_ChatingPrograme
## About Project.
#### 개발 환경
- Visual Studio 2022
- Workbench 8.0
- C++
- TCP/IP
- MYSQL
---
## 주요 기능
### Packet
- 해더(4byte) + 데이터(0 ~ 1024byte)
    - 해더 : 패킷 크기(2byte) + 패킷 코드(2byte)

### TCP Server
- 단일 쓰레드 사용
  1. 연결 확인
  2. while
      - 패킷 헤더의 정보 수신 대기 
      - 해더의 정보를 기반으로 패킷 데이터 수집
      - 패킷 코드에 따라 switch 문 실행

### DB Server
- DB Table
  - User Info

    유저의 ID, 닉네임, 비밀번호 저장 및 관리
    
  - Chat Log

    채팅 내용 저장

### Client
- 멀티 쓰레드 사용
    - Recv Thread

      패킷 코드에 따른 CurrentPacketState 변수 값 대입
      
    - Send Thread
 
      CurrentPacketState 변수 값에 따른 switch 문 실행










 
<details>
<summary>접기/펼치기</summary>
접은 내용(ex 소스 코드)
</details>
