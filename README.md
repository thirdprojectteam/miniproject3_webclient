# miniproject3_webclient 
 한국소프트웨어 협회 - 미니프로젝트3 웹클라이언트
 <hr>
 <h3>라이브러리</h3> 
 - QT += core gui network charts websockets

## Qt Charts 설치 (Qt 6 기준)

Qt 6에서는 **Qt Charts**가 Add-ons 모듈이므로, 기본 설치에 없을 수 있습니다.  
다음 절차로 설치하세요.

## 설치 방법
1. **Qt Maintenance Tool** 실행  
2. **Add or remove components** 선택  
3. 사용하는 Qt 6 버전 아래에서  
   **Add-ons → Qt Charts** 체크  
4. 설치 완료 후 빌드 환경 재시작

> 💡 리눅스 패키지 사용 시:
> ```bash
> sudo apt install qt6-charts-dev
> ```

