#include <iostream>
#include <list>
#include <string>
using namespace std;


int main()
{




    return 0;
}


/*

    - list는 모든 삽입 삭제가 상수 시간이다. 이중 연결 리스트이기 때문
    - 대신 개별 항목에 접근하는데는 다소 느린 시간을 가지고 있다. 

    - [] 와 같은 연산자 오버로딩을 지원하지 않는다. 
    - list에서 개별 요소를 접근하려면 front() , back() 뿐이다. 각각 참조형을 리턴한다. 
    다른 항목들(끝과 시작이 아닌 것들)은 iterator를 이용해 접근해야 한다. 즉 begin() , end() , cbegin() , cend() 이런 것들로 해야 된다는 소리

    - 앞서 설명한 것처럼 iterator를 이용해 포인터 연산을 하듯이 접근할 수 있다. ++iter , --iter 이렇게 말이다   근데 iter += 2 뭐 이렇게 점프는 불가능 하다.
    - 삽입과 삭제가 빈번하다면 list를 고려할 수 있다. 다만 접근 하는 성능은 느리다는 것을 알아야 한다. 

    - 벡터와 달리 메모리 관리 구조를 노출하지 않는다. 그래서 capacity() , reserve() 를 지원하지 않는다. size() , resize() , empty() 와 같은 함수는 벡터처럼 지원한다.


    - 삽입 삭제가 빠른 리스트의 특별한 기능 몇가지가 있다
    1. splice() : 원본 리스트에 변경을 가한다. 
    2. remove() : 특정 항목을 제거 한다. remove_if() 특정 조건의 항목을 제거 한다.
    3. unique() : 중복 되는거 제거 한다. 
    4. merge() , sort() , reverse() 

    - forward_list 는 단향방이다. iter가 -- 가 될 수 없다. 특정 항목에 접근해야 된다면 반드시 이전 항목을 거쳐야 된다는 소리다. 



*/