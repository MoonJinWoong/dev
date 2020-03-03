#include <iostream>
#include <deque>
#include <string>
using namespace std;


int main()
{
    deque<int> v;

   


    return 0;
}


/*
    - 양쪽에서 입출력이 가능한 큐. (근데 거의 안쓰는 느낌임). 벡터와 기능적으로 거의 동일하다
    - 벡터와의 차이점
    1. 연속된 메모리에 저장될 필요가 없음. 
    2. 벡터와 달리 덱은 앞뒤 삽입, 삭제 모두 상수 시간을 가진다(벡터는 뒤만)
    3. front 관련 메서드가 있다. 
    4. capacity() , reserve() 같은 내부 메모리를 관리할 수 있는 함수가 없다. 
*/