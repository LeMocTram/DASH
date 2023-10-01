#Get Ubuntu image base

FROM ubuntu:latest as server-image

#Label of the images

LABEL description="Server container"

#Update and install g++ on base image
RUN apt-get -y update && apt-get install -y build-essential \
    && apt-get install -y libmysqlclient-dev \
    && apt-get install -y vim

#Copy folder of souce code server to Docker images
COPY . /source
#Working directory
WORKDIR /source/server

#compile source with C++
RUN g++ -o server server.cpp -lmysqlclient
#Run the application
#CMD ["./server"]
CMD ["tail", "-f" ,"/dev/null"]

EXPOSE 12345

#Second image base

# cat Dockerfile
FROM mysql:latest as database-image


#RUN chown -R mysql:root /var/lib/mysql/

ARG MYSQL_DATABASE=CHATSERVICE
ARG MYSQL_USER=server 
ARG MYSQL_PASSWORD=123456
ARG MYSQL_ROOT_PASSWORD=123456

ENV MYSQL_ROOT_PASSWORD=$MYSQL_ROOT_PASSWORD
ENV MYSQL_DATABASE=$MYSQL_DATABASE
ENV MYSQL_USER=$MYSQL_USER
ENV MYSQL_PASSWORD=$MYSQL_PASSWORD

COPY ./database/chatService.sql /docker-entrypoint-initdb.d

#Working directory

EXPOSE 3306
