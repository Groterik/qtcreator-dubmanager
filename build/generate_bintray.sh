#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJ_DIR="$(cd "${DIR}/.." && pwd )"
PROJ_NAME="$(basename ${PROJ_DIR})"
CONF_FILE="${PROJ_DIR}/.bintray.json"
CURR_DATE="$(date +%Y-%m-%d)"


cp "${PROJ_DIR}/.bintray.json.in" "${CONF_FILE}"

sed -i -e "s/{{project}}/${PROJ_NAME}/g" "${CONF_FILE}"
sed -i -e "s/{{os}}/${TRAVIS_OS_NAME}/g" "${CONF_FILE}"
sed -i -e "s/{{version}}/${VERSION}/g" "${CONF_FILE}"
sed -i -e "s/{{build}}/${TRAVIS_BUILD_NUMBER}/g" "${CONF_FILE}"
sed -i -e "s/{{branch}}/${TRAVIS_BRANCH}/g" "${CONF_FILE}"
sed -i -e "s/{{commit}}/${TRAVIS_COMMIT}/g" "${CONF_FILE}"
sed -i -e "s/{{date}}/${CURR_DATE}/g" "${CONF_FILE}"

echo "Generated ${CONF_FILE}:"
cat "${CONF_FILE}"

