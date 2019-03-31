FROM ubuntu:16.04
LABEL Author oskar.oestby@gmail.com

RUN apt-get update -qq && apt-get upgrade -qqy \
&& apt-get install -qqy gcc \
cmake

WORKDIR /project