#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. 요구 사항에 따른 Student 구조체 정의
typedef struct {
    char name[12];
    int kor;
    int eng;
    int math;
    int total;    // 총점
    double avg;   // 평균
    int rank;     // 등수
} Student;

// 연결 리스트의 노드 구조체 정의
typedef struct Node {
    Student data;       // 학생 정보를 저장할 Student 구조체
    struct Node* p_next; // 다음 노드를 가리킬 포인터
} Node;

// 전역 변수 (연결 리스트의 시작과 끝)
Node* p_head = NULL;
Node* p_tail = NULL;
const char* FILENAME = "students.dat";

// --- 함수 선언 ---
void AddStudent(const Student* new_student);
void LoadDataFromFile();
void SaveDataToFile();
void InputNewStudent();
void CalculateStatsAndRank();
void PrintAllStudents();
void CleanUpList();
void DisplayMenu();

// --- 함수 구현 ---

/**
 * @brief 새로운 학생 데이터를 연결 리스트에 추가합니다.
 * @param new_student 추가할 학생 데이터의 포인터
 */
void AddStudent(const Student* new_student) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("메모리 할당 실패!\n");
        exit(1);
    }

    // 데이터 복사
    new_node->data = *new_student;
    new_node->p_next = NULL;

    if (p_head == NULL) {
        p_head = new_node;
        p_tail = new_node;
    }
    else {
        p_tail->p_next = new_node;
        p_tail = new_node;
    }
}

/**
 * @brief 이진 파일에서 데이터를 읽어와 연결 리스트에 로드합니다. (메뉴 1)
 */
void LoadDataFromFile() {
    FILE* fp;
    Student s;

    // 파일 열기 (이진 읽기 모드)
    if (fopen_s(&fp, FILENAME, "rb") != 0) {
        printf("--- 파일 '%s'을(를) 열 수 없습니다. 새 파일로 시작합니다. ---\n", FILENAME);
        return;
    }

    // 기존 데이터 정리
    CleanUpList();

    printf("\n--- 파일에서 데이터 읽는 중... ---\n");
    // 파일 끝까지 반복하며 Student 구조체 단위로 읽기
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        AddStudent(&s);
    }

    fclose(fp);
    printf("--- 데이터 로드 완료. 현재 %d명의 학생 정보가 있습니다. ---\n", GetStudentCount());
}

/**
 * @brief 현재 연결 리스트의 모든 데이터를 이진 파일에 저장합니다. (메뉴 3)
 */
void SaveDataToFile() {
    if (p_head == NULL) {
        printf("저장할 학생 데이터가 없습니다.\n");
        return;
    }

    FILE* fp;
    // 파일 열기 (이진 쓰기 모드)
    if (fopen_s(&fp, FILENAME, "wb") != 0) {
        printf("--- 파일 '%s'을(를) 저장할 수 없습니다! ---\n", FILENAME);
        return;
    }

    Node* current = p_head;
    int count = 0;
    while (current != NULL) {
        // Student 구조체 단위로 파일에 쓰기
        fwrite(&(current->data), sizeof(Student), 1, fp);
        current = current->p_next;
        count++;
    }

    fclose(fp);
    printf("\n--- 총 %d명의 학생 정보가 파일 '%s'에 성공적으로 저장되었습니다. ---\n", count, FILENAME);
}

/**
 * @brief 사용자로부터 새로운 학생 정보를 입력받아 연결 리스트에 추가합니다. (메뉴 2)
 */
void InputNewStudent() {
    Student s;

    printf("\n--- 새로운 학생 정보 입력 ---\n");
    printf("이름 (최대 11자): ");
    scanf_s("%11s", s.name, (unsigned int)sizeof(s.name)); // scanf_s 사용 시 문자열 길이 지정

    printf("국어 점수: ");
    while (scanf_s("%d", &s.kor) != 1 || s.kor < 0 || s.kor > 100) {
        printf("잘못된 점수입니다. 0~100 사이의 숫자를 입력하세요: ");
        while (getchar() != '\n'); // 입력 버퍼 비우기
    }
    printf("영어 점수: ");
    while (scanf_s("%d", &s.eng) != 1 || s.eng < 0 || s.eng > 100) {
        printf("잘못된 점수입니다. 0~100 사이의 숫자를 입력하세요: ");
        while (getchar() != '\n');
    }
    printf("수학 점수: ");
    while (scanf_s("%d", &s.math) != 1 || s.math < 0 || s.math > 100) {
        printf("잘못된 점수입니다. 0~100 사이의 숫자를 입력하세요: ");
        while (getchar() != '\n');
    }

    // 총점과 평균은 즉시 계산하지 않고, 메뉴 4에서 일괄 계산
    s.total = 0;
    s.avg = 0.0;
    s.rank = 0;

    AddStudent(&s);
    printf("학생 '%s' 정보가 임시 목록에 추가되었습니다. (저장하려면 3번 선택)\n", s.name);
}

/**
 * @brief 연결 리스트를 순회하며 총점, 평균을 계산하고 등수를 매깁니다. (메뉴 4의 전처리)
 */
void CalculateStatsAndRank() {
    if (p_head == NULL) {
        return;
    }

    Node* current = p_head;

    // 1단계: 총점 및 평균 계산
    while (current != NULL) {
        current->data.total = current->data.kor + current->data.eng + current->data.math;
        current->data.avg = (double)current->data.total / 3.0;
        current->data.rank = 1; // 초기 등수를 1로 설정
        current = current->p_next;
    }

    // 2단계: 등수 매기기 (총점을 기준으로 비교)
    Node* compare_target = p_head;
    while (compare_target != NULL) {
        Node* comparison = p_head;
        while (comparison != NULL) {
            if (comparison->data.total > compare_target->data.total) {
                compare_target->data.rank++;
            }
            comparison = comparison->p_next;
        }
        compare_target = compare_target->p_next;
    }
}

/**
 * @brief 성적 확인 결과를 출력합니다. (메뉴 4)
 */
void PrintAllStudents() {
    if (p_head == NULL) {
        printf("\n--- 출력할 학생 데이터가 없습니다. (1번 또는 2번 메뉴 이용) ---\n");
        return;
    }

    // 등수 및 통계 계산 수행
    CalculateStatsAndRank();

    printf("\n==================================================================\n");
    printf("| 이름       | 국어 | 영어 | 수학 | 총점 | 평균     | 등수 |\n");
    printf("==================================================================\n");

    Node* current = p_head;
    while (current != NULL) {
        Student s = current->data;
        printf("| %-10s | %-4d | %-4d | %-4d | %-4d | %-8.2f | %-4d |\n",
            s.name, s.kor, s.eng, s.math, s.total, s.avg, s.rank);
        current = current->p_next;
    }
    printf("==================================================================\n");
}

/**
 * @brief 현재 연결 리스트의 모든 노드에 할당된 메모리를 해제합니다.
 */
void CleanUpList() {
    Node* current = p_head;
    Node* next;
    while (current != NULL) {
        next = current->p_next;
        free(current);
        current = next;
    }
    p_head = NULL;
    p_tail = NULL;
}

/**
 * @brief 현재 리스트에 있는 학생 수를 반환합니다.
 */
int GetStudentCount() {
    int count = 0;
    Node* current = p_head;
    while (current != NULL) {
        count++;
        current = current->p_next;
    }
    return count;
}


/**
 * @brief 메뉴 화면을 표시합니다.
 */
void DisplayMenu() {
    printf("\n[Menu]\n");
    printf("1. .dat 파일에서 데이터 읽기\n");
    printf("2. 추가 학생 정보 입력\n");
    printf("3. .dat 파일 저장\n");
    printf("4. 성적 확인 (평균 계산 등)\n");
    printf("5. 종료\n");
    printf("-------------------\n");
    printf("선택(1~5): ");
}


// --- 메인 함수 ---
int main(void) {
    int choice;

    // 프로그램 시작 시, 데이터 파일을 먼저 로드 시도
    LoadDataFromFile();

    while (1) {
        DisplayMenu();
        if (scanf_s("%d", &choice) != 1) {
            printf("\n--- 잘못된 입력입니다. 1~5 사이의 숫자를 입력하세요. ---\n");
            while (getchar() != '\n'); // 입력 버퍼 비우기
            continue;
        }

        switch (choice) {
        case 1:
            // .dat 파일에서 데이터 읽기
            LoadDataFromFile();
            break;
        case 2:
            // 추가 학생 정보 입력
            InputNewStudent();
            break;
        case 3:
            // .dat 파일 저장
            SaveDataToFile();
            break;
        case 4:
            // 성적 확인 (출력 전 통계 및 등수 계산 포함)
            PrintAllStudents();
            break;
        case 5:
            // 종료
            printf("\n--- 프로그램 종료. 메모리를 정리합니다. ---\n");
            CleanUpList();
            return 0;
        default:
            printf("\n--- 잘못된 메뉴 선택입니다. 1에서 5 사이를 선택하세요. ---\n");
            break;
        }
    }

    return 0;
}