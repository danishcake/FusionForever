echo "The following will be removed:"
git clean -x -f -d -n
echo "Do you want to remove these untracked files?"
read -p "y/n: " CHOICE
case "$CHOICE" in
	"y")
		echo "Cleaning all untracked files..."
		git clean -x -f -d
	;;
	"n")
		echo "Not cleaning files"
	;;
esac