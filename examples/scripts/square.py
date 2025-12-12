import sys

def main():
    # Simple script to square the input number
    if len(sys.argv) > 1:
        try:
            # Handle potential quoted args from the node
            arg = sys.argv[1].replace('"', '').replace("'", "")
            val = float(arg)
            print(f"{val * val:.2f}")
        except ValueError:
            print(f"Error: '{sys.argv[1]}' is not a number", file=sys.stderr)
            sys.exit(1)
    else:
        print("Error: No input provided", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
