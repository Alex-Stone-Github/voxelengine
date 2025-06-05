FROM ubuntu

RUN apt-get update

WORKDIR /app
COPY . .

RUN apt install build-essential -y
RUN apt install libsdl2-dev -y
RUN apt install libglm-dev -y
RUN apt install libglew-dev -y
#RUN make

CMD ["sh", "bootstrap.sh"]