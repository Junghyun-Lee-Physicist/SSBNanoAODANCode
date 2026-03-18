# jsonconfig - JSON 기반 설정 모듈 (Beta)

SSBNanoAODANCode에서 기존 TextReader와 **병렬로** 작동하는 JSON 설정 모듈입니다.
`the-xs-barn` 레포지토리의 JSON 포맷과 호환됩니다.

## 구조

```
jsonconfig/
├── json.hpp                 # nlohmann/json single-header (의존성)
├── JsonConfigReader.hpp     # JSON config reader 헤더
├── JsonConfigReader.cpp     # JSON config reader 구현
├── XSecJsonLoader.hpp       # Cross section JSON loader 헤더
├── XSecJsonLoader.cpp       # Cross section JSON loader 구현
└── README.md                # 이 파일
```

## 빌드 방법

`Makefile_ssb`에 이미 포함되어 있습니다. 기존과 동일하게 빌드하면 됩니다:

```bash
make -f Makefile_ssb clean && make -f Makefile_ssb
```

## 실행 방법

기존 실행 커맨드에 2개의 인자를 추가합니다 (선택 사항):

```bash
# 기존 방식 (TextReader만 사용, 변경 없음)
./ssb_analysis filelist output.root ULSummer20/UL2018/dimuon.config . 2018 -1 branchlist.txt

# JSON 설정 동시 사용 (beta - TextReader + JSON 동시 출력 비교)
./ssb_analysis filelist output.root ULSummer20/UL2018/dimuon.config . 2018 -1 branchlist.txt \
    ULSummer20/UL2018/dimuon.json \
    /path/to/the-xs-barn/data/cpv_samples_2018UL.json
```

- **8번째 인자**: JSON config 파일 경로 (`configs/` 디렉토리 기준 상대 경로)
- **9번째 인자**: JSON XSec 파일 경로 (절대 경로 또는 상대 경로)

## JSON Config 파일 (configs/ 디렉토리)

기존 `.config` 파일의 JSON 버전입니다. 키 이름이 동일하므로 TextReader와 1:1 비교가 가능합니다.

```json
{
  "CenOfEn": "13TeV",
  "Luminosity": 59832422396.795,
  "Channel": "dimuon",
  "MuonPt_cut": 20.0,
  "dileptrigger": ["HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8"]
}
```

## JSON XSec 파일 (the-xs-barn/data/ 디렉토리)

`cpv_samples_XXXX.json` 파일은 기존 `xsecAndsample/*.txt`의 JSON 버전입니다.

```json
{
  "TTbar_Signal": {
    "das_path": "/TTTo2L2Nu_TuneCP5_.../NANOAODSIM",
    "total_events": 145020000,
    "positive_events": 144434424,
    "negative_events": 585576,
    "effective_events": 143848848,
    "cross_section_pb": 831.76,
    "branching_fraction": 0.10706,
    "frac_neg_weight": 0.004038,
    "comment": "ttbar dilepton signal"
  }
}
```

## Analyzer에서 값 가져오기

### 방법 1: TextReader 호환 인터페이스 (기존 코드 그대로 동작)

`JsonConfigReader`는 TextReader와 동일한 메서드를 제공합니다:

```cpp
// TextReader 방식 (기존 코드)
double lumi = SSBConfReader->GetNumber("Luminosity");
std::string channel = SSBConfReader->GetText("Channel");
bool dojer = SSBConfReader->GetBool("DoJER");
int ntrig = SSBConfReader->Size("dileptrigger");
std::string trig = SSBConfReader->GetText("dileptrigger", 1); // 1-based index

// JsonConfigReader 방식 (동일한 인터페이스)
double lumi = SSBJsonConfReader->GetNumber("Luminosity");
std::string channel = SSBJsonConfReader->GetText("Channel");
bool dojer = SSBJsonConfReader->GetBool("DoJER");
int ntrig = SSBJsonConfReader->Size("dileptrigger");
std::string trig = SSBJsonConfReader->GetText("dileptrigger", 1); // 1-based index
```

### 방법 2: 직접 JSON 접근 (새로운 직관적 API)

```cpp
// 타입 안전한 직접 접근
double lumi = SSBJsonConfReader->GetAs<double>("Luminosity");
std::string channel = SSBJsonConfReader->GetAs<std::string>("Channel");
int threshold = SSBJsonConfReader->GetAs<int>("PUIDPtThreshold");

// 기본값 지정 가능
double met_cut = SSBJsonConfReader->GetAs<double>("MET_cut", 40.0);

// 키 존재 여부 확인
if (SSBJsonConfReader->Has("DoJER")) { ... }

// 원본 JSON 객체 직접 접근
const auto& triggers = SSBJsonConfReader->Get("dileptrigger");
for (const auto& t : triggers) {
    std::cout << t.get<std::string>() << std::endl;
}
```

### 방법 3: XSec JSON 접근

```cpp
// 샘플 정보 가져오기
if (SSBJsonXSecLoader->HasSample("TTbar_Signal")) {
    double xsec = SSBJsonXSecLoader->GetXSec("TTbar_Signal");
    double br = SSBJsonXSecLoader->GetBR("TTbar_Signal");
    int eff_evt = SSBJsonXSecLoader->GetEffectiveEvents("TTbar_Signal");

    // 전체 정보 구조체
    const auto& info = SSBJsonXSecLoader->GetSample("TTbar_Signal");
    std::cout << info.das_path << std::endl;
    std::cout << info.positive_events << std::endl;
    std::cout << info.negative_events << std::endl;
}

// 모든 샘플 이름
auto names = SSBJsonXSecLoader->GetSampleNames();
```

## 현재 상태 (Beta)

- TextReader는 **전혀 변경되지 않았습니다**
- JSON 설정은 TextReader와 **동시에** 로드되며, 출력을 비교할 수 있습니다
- `MCSF()` 함수에서 TextReader 기반 mc_sf와 JSON 기반 mc_sf를 동시에 출력합니다
- JSON 인자를 넘기지 않으면 기존과 **100% 동일하게** 작동합니다

## 사용 가능한 JSON 파일

### Config (SSBNanoAODANCode/configs/)
- `ULSummer20/UL2018/dimuon.json`
- `ULSummer20/UL2018/dielec.json`
- `ULSummer20/UL2018/muelec.json`

### XSec (the-xs-barn/data/)
- `cpv_samples_2016PreVFP_UL.json`
- `cpv_samples_2016PostVFP_UL.json`
- `cpv_samples_2017UL.json`
- `cpv_samples_2018UL.json`
