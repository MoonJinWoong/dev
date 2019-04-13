
* 프로젝트 구조  
Host : CUI 기반 프로젝트  
LogicLayer: 정적 라이브러리 -> Host에서 참조  
NetworkLayer: 정적 라이브러리 -> LogicLayer에서 참조  


* To Do
1. 로직에서 유저 , 유저매니저 클래스 생성 및 테스트   
2. 현재 그냥 접속 상태에서 -> 로그인하는 방식으로 Logic 수정  
3. packetProcess를 로직에서 하는 것으로 변경   
4. NetworkLayer 구조 변경 -> 상속, 템플릿 사용  
