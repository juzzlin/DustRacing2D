VERSION=${DUSTRAC_RELEASE_VERSION?"is not set."}
DEBIAN_VERSION=$VERSION-1
rm -rf *${VERSION}
cp ../sourceforge-git/dustrac-code/dustrac-$VERSION.tar.gz .
mv dustrac-$VERSION.tar.gz dustrac_$VERSION.orig.tar.gz 
tar xzvf dustrac_$VERSION.orig.tar.gz
cd dustrac-$VERSION
cp -rv packaging/debian .
debuild -S -sa && cd .. && dput ppa:jussi-lind/dustrac "dustrac_${DEBIAN_VERSION}_source.changes"

