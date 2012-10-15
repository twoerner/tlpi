#!/bin/bash

mkdir dir1 dir2

cat > dir1/xyz <<EOF
#!/bin/bash
echo "dir1"
EOF
cat > dir2/xyz <<EOF
#!/bin/bash
echo "dir2"
EOF

chmod +x dir1/xyz dir2/xyz

export PATH=/usr/local/bin:/usr/bin:/bin:./dir1:./dir2
./listing_27-3 xyz

rm -fr dir1 dir2
