# Safe_Delete

# Compile
gcc -o safedelete main.c safedelete.c

# Test workflow
echo "Test content" > testfile.txt
./safedelete -d testfile.txt                 # Secure delete
./safedelete -l                              # List trash (note index)
./safedelete -r 1                            # Recover file
./safedelete -c 0                            # Cleanup trash immediately

# Verify file recovery
cat testfile.txt
