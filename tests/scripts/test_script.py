import sys

def main():
    print("Python Script Output")
    if len(sys.argv) > 1:
        print(f"Arguments: {sys.argv[1:]}")
    
if __name__ == "__main__":
    main()
