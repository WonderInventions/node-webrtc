from pathlib import Path
import sys


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: patch-vs-toolchain.py <path-to-vs_toolchain.py>", file=sys.stderr)
        return 2

    path = Path(sys.argv[1])
    text = path.read_text(encoding="utf-8")
    old = """      else:
        raise Exception('%s not found in "%s"\\r\\nYou must install '
                        'Windows 10 SDK version %s including the '
                        '"Debugging Tools for Windows" feature.' %
                        (debug_file, full_path, SDK_VERSION))
"""
    new = """      else:
        print('%s not found in "%s"; skipping debugger runtime copy.' %
              (debug_file, full_path))
        return
"""
    if old not in text:
        print(f"expected block not found in {path}", file=sys.stderr)
        return 1

    path.write_text(text.replace(old, new, 1), encoding="utf-8")
    print(f"patched {path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
