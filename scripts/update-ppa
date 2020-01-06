VERSION=${DUSTRAC_RELEASE_VERSION?"is not set."}

# $VERSION-1 for xenial
# $VERSION-2 for artful

DEBIAN_VERSION=$VERSION-1
rm -rf *${VERSION}*
cp ../DustRacing2D/dustrac-$VERSION.tar.gz .
tar xzvf dustrac-$VERSION.tar.gz
mv dustrac-$VERSION dustrac-$DEBIAN_VERSION
cd dustrac-$DEBIAN_VERSION
cp -rv packaging/debian .
debuild -S -sa && cd .. && dput ppa:jussi-lind/dustrac "dustrac_${DEBIAN_VERSION}_source.changes"

