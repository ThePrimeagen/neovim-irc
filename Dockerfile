FROM ubuntu:20.04
WORKDIR /app
ENV DEBIAN_FRONTEND noninteractive
RUN ls -la
RUN apt update && apt-get install -y build-essential cmake
COPY . .
RUN mkdir -p /app/build
RUN chown -R nobody:nogroup /app
USER nobody
WORKDIR /app/build
RUN cmake ..
RUN make
EXPOSE 1337/tcp
CMD ["./src/neovim-irc"]


