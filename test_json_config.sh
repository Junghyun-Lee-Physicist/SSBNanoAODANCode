#!/bin/bash
###############################################################################
# test_json_config.sh
# JSON config 비교 테스트 스크립트 (로컬 실행용, condor 없이)
#
# 사용법:
#   ./test_json_config.sh [ERA] [CHANNEL] [NEVT]
#
# 예시:
#   ./test_json_config.sh                          # 기본: UL2018 muelec, 100 이벤트
#   ./test_json_config.sh UL2017 dimuon 50         # UL2017 dimuon, 50 이벤트
#   ./test_json_config.sh UL2016PreVFP dielec 200  # UL2016PreVFP dielec, 200 이벤트
#
# 설명:
#   TextReader (.config)와 JSON (.json) 설정값을 동시에 로드하여
#   side-by-side 비교 프린트를 출력합니다.
#   실제 이벤트 루프도 돌기 때문에 input 파일과 ROOT 파일이 필요합니다.
#   ROOT 파일이 없으면 빌드만 테스트합니다.
###############################################################################

set -e

# 기본값
ERA="${1:-UL2018}"
CHANNEL="${2:-muelec}"
NEVT="${3:-100}"

# 디렉토리
BASEDIR="$(cd "$(dirname "$0")" && pwd)"
CONFIGDIR="configs/ULSummer20"
XSECDIR="${BASEDIR}/../the-xs-barn/data"

echo "=============================================="
echo " JSON Config 비교 테스트"
echo "=============================================="
echo " ERA     : ${ERA}"
echo " CHANNEL : ${CHANNEL}"
echo " NEVT    : ${NEVT}"
echo "=============================================="

# ERA → xsec JSON 파일 매핑
case "${ERA}" in
  UL2018)          XSEC_JSON="${XSECDIR}/cpv_samples_2018UL.json" ;;
  UL2017)          XSEC_JSON="${XSECDIR}/cpv_samples_2017UL.json" ;;
  UL2016PreVFP)    XSEC_JSON="${XSECDIR}/cpv_samples_2016PreVFP_UL.json" ;;
  UL2016PostVFP)   XSEC_JSON="${XSECDIR}/cpv_samples_2016PostVFP_UL.json" ;;
  *) echo "ERROR: Unknown ERA '${ERA}'"; exit 1 ;;
esac

# config 파일 경로
TEXT_CONFIG="${CONFIGDIR}/${ERA}/${CHANNEL}.config"
JSON_CONFIG="${CONFIGDIR}/${ERA}/${CHANNEL}.json"

# 파일 존재 확인
echo ""
echo "[CHECK] Text config: ${TEXT_CONFIG}"
if [ -f "${BASEDIR}/${TEXT_CONFIG}" ]; then
  echo "  -> OK"
else
  echo "  -> NOT FOUND!"; exit 1
fi

echo "[CHECK] JSON config: ${JSON_CONFIG}"
if [ -f "${BASEDIR}/${JSON_CONFIG}" ]; then
  echo "  -> OK"
else
  echo "  -> NOT FOUND!"; exit 1
fi

echo "[CHECK] XSec JSON: ${XSEC_JSON}"
if [ -f "${XSEC_JSON}" ]; then
  echo "  -> OK"
else
  echo "  -> NOT FOUND (xsec 비교는 건너뜁니다)"
  XSEC_JSON=""
fi

# 빌드
echo ""
echo "=============================================="
echo " 빌드 중... (make -f Makefile_ssb)"
echo "=============================================="
cd "${BASEDIR}"
make -f Makefile_ssb -j$(nproc) 2>&1

echo ""
echo "빌드 성공!"

# input 파일 찾기
INPUT_FILE=""
for candidate in \
  "input/${ERA}_TTbar_Signal_1.list" \
  "input/TTbar_Signal_1.list" \
  "input/TTJets_Signal_1.list"; do
  if [ -f "${BASEDIR}/${candidate}" ]; then
    INPUT_FILE="$(basename ${candidate})"
    echo "[INPUT] ${candidate} 사용"
    break
  fi
done

if [ -z "${INPUT_FILE}" ]; then
  echo ""
  echo "=============================================="
  echo " WARNING: input 파일을 찾을 수 없습니다."
  echo " 빌드는 성공했으나, 실행 테스트는 건너뜁니다."
  echo ""
  echo " 직접 실행하려면:"
  echo "   ./ssb_analysis <input.list> <output.root> ${TEXT_CONFIG} ./ ${ERA} ${NEVT} branch_list.txt ${JSON_CONFIG} ${XSEC_JSON}"
  echo "=============================================="
  exit 0
fi

# branchlist 찾기
BRANCHLIST="branch_list.txt"
for bl in "branch_list.txt" "branch_list_2016.txt"; do
  if [ -f "${BASEDIR}/branchlist/${bl}" ]; then
    BRANCHLIST="${bl}"
    break
  fi
done

# 출력 파일
OUTPUT="test_output_${ERA}_${CHANNEL}.root"

echo ""
echo "=============================================="
echo " 실행 중..."
echo "=============================================="
echo " CMD: ./ssb_analysis ${INPUT_FILE} ${OUTPUT} ${TEXT_CONFIG} ./ ${ERA} ${NEVT} ${BRANCHLIST} ${JSON_CONFIG} ${XSEC_JSON}"
echo ""

./ssb_analysis \
  "${INPUT_FILE}" \
  "${OUTPUT}" \
  "${TEXT_CONFIG}" \
  "./" \
  "${ERA}" \
  "${NEVT}" \
  "${BRANCHLIST}" \
  "${JSON_CONFIG}" \
  "${XSEC_JSON}"

echo ""
echo "=============================================="
echo " 완료!"
echo " 출력 파일: ${OUTPUT}"
echo "=============================================="
