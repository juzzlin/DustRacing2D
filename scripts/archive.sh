VERSION=$(git log | grep -m 1 "^\s*[0-9]\.[0-9]*\.[0-9]*" | awk '{print $1}')
git archive --format=tar --prefix=dustrac-$VERSION/ HEAD | gzip > dustrac-$VERSION.tar.gz
git archive --format=zip --prefix=dustrac-$VERSION/ HEAD > dustrac-$VERSION.zip

