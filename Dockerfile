FROM ubuntu:18.04
LABEL maintainer 'Daniel M. Lambea <dmlambea@gmail.com>'

RUN apt update && \
	apt install -y --no-install-recommends build-essential debhelper dkms fakeroot && \
	rm -rf /var/lib/apt/lists/*

COPY  /container_scripts/dkmsbuild.sh /usr/local/bin/
RUN chmod 755 /usr/local/bin/dkmsbuild.sh && \
	chgrp nogroup /usr/src /var/lib/dkms && \
	chmod g+rwxs /usr/src /var/lib/dkms

USER nobody

VOLUME /src
VOLUME /build
VOLUME /kernelsrc

CMD ['/usr/local/bin/dkmsbuild.sh']
