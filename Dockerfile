# -*- mode: Dockerfile -*-

FROM ghcr.io/feelpp/feelpp:noble

USER root
COPY . /home/feelpp/
RUN ls -lrtR /home/feelpp

RUN dpkg -i /home/feelpp/*.deb

USER feelpp

HEALTHCHECK --interval=30 --timeout=10s --start-period=5s --retries=3 \
    CMD feelpp_app_io --help > /dev/null 2>&1 || exit 1
