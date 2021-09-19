#!/bin/bash
set -ex

mkdir -p /opt/nslcore
curl https://ip-arch.jp/unsupported/nslcore-x86_64-linux-20210228-90.tar.gz | tar xvz -C /opt/nslcore
cp nsl2vl.patch /opt/nslcore/linux/x86_64
pushd /opt/nslcore/linux/x86_64 &> /dev/null
	patch < ./nsl2vl.patch
popd &> /dev/null
