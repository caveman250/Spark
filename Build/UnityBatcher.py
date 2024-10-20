import os
import sys


def parse_conf_file(path):
    excluded_files = []
    root = ""
    file = open(path)
    for line in file:
        if line.startswith("Root"):
            root = line[len("Root "):len(line)-1]
        elif line.startswith("ExcludeFile"):
            excluded_files.append(line[len("ExcludeFile "):len(line)-1])
    file.close()

    output_dir = root + "/../unity_generated/"

    return root, excluded_files, output_dir


def create_unity_files(conf_path, platform):
    root_dir, excluded_files, output_dir = parse_conf_file(conf_path)

    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    for root, dirs, files in os.walk(root_dir):
        file_index = 0
        new_files = []
        file_counter = 0

        if platform == "Linux":
            if "platform/windows" in root:
                continue
        elif platform == "Windows":
            if "platform/linux" in root:
                continue

        for file in files:
            excluded = False
            excluded_name = ""
            for exclude in excluded_files:
                if exclude in os.path.join(root, file):
                    excluded = True
                    excluded_name = exclude
                    break

            if excluded:
                new_name = excluded_name.replace("/", "_")

                old_file_handle = open(output_dir + new_name)
                old_contents = old_file_handle.read()
                old_file_handle.close()

                if old_contents != f"#include \"{excluded_name}\"":
                    print(f"stale file {new_name}")
                    output_handle = open(output_dir + new_name, "w+")
                    output_handle.write(f"#include \"{excluded_name}\"")
                    output_handle.close()
                continue

            if file.endswith(".cpp"):
                if file_index >= len(new_files):
                    new_files.append("")
                full_path = os.path.abspath(os.path.join(root, file))
                new_files[file_index] += "#include \"" + full_path + "\"\n"
                file_counter += 1
                if file_counter > 10:
                    file_index += 1
                    file_counter = 0

        file_counter = 0
        for new_file in new_files:
            new_name = root.replace(root_dir, "src/")
            new_name = new_name.replace("//", "_")
            new_name = new_name.replace("/", "_")
            if new_name.endswith("_"):
                new_name = new_name[0:len(new_name)-1]
            if file_counter > 0:
                new_name += f"_{file_counter}.cpp"
            else:
                new_name += ".cpp"

            old_contents = ""
            if (os.path.exists(output_dir + new_name)):
                old_file_handle = open(output_dir + new_name)
                old_contents = old_file_handle.read()
                old_file_handle.close()

            if old_contents != new_file:
                print(f"stale file {new_name}")
                output_handle = open(output_dir + new_name, "w+")
                output_handle.write(new_file)
                output_handle.close()
            file_counter += 1

def main():
    print("-- Generating unity files")

    conf_file = sys.argv[1]
    platform = sys.argv[2]
    create_unity_files(conf_file, platform)

if __name__ == '__main__':
    main()