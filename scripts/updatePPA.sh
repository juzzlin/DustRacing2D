VERSION=${DUSTRAC_RELEASE_VERSION?"is not set."}

# $VERSION-1 for trusty
# $VERSION-2 for utopic
# $VERSION-3 for vivid

DEBIAN_VERSION=$VERSION-3
rm -rf *${VERSION}*
cp ../sourceforge-git/dustrac-code/dustrac-$VERSION.tar.gz .
tar xzvf dustrac-$VERSION.tar.gz
mv dustrac-$VERSION dustrac-$DEBIAN_VERSION
cd dustrac-$DEBIAN_VERSION
cp -rv packaging/debian .
debuild -S -sa && cd .. && dput ppa:jussi-lind/dustrac "dustrac_${DEBIAN_VERSION}_source.changes"

