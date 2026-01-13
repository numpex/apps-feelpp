# -*- mode: Dockerfile -*-

FROM ghcr.io/feelpp/feelpp:ubuntu-24.04

USER root
COPY . /home/feelpp/
RUN ls -lrtR /home/feelpp

RUN dpkg -i /home/feelpp/*.deb

USER feelpp

HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD feelpp_app_io --help > /dev/null 2>&1 || exit 1
