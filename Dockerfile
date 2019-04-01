FROM ubuntu:16.04 AS build
LABEL Author oskar.oestby@gmail.com

RUN apt-get update -qq && apt-get upgrade -qqy \
&& apt-get install -qqy build-essential \
gcc \
cmake \
make

WORKDIR /project

COPY . .
RUN ./build.sh

FROM ubuntu:16.04
WORKDIR /app
COPY --from=build /project/build/sim_server /app
EXPOSE 6060/tcp 6060/tcp
ENTRYPOINT [ "/app/sim_server" ]