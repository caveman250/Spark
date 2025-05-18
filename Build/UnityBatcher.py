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

    output_dir = root + "/../unity/"

    return root, excluded_files, output_dir

def write_output_files(files, file_extension, root, root_dir, output_dir):
    file_counter = 0
    for new_file in files:
        new_name = root.replace(root_dir, "src/")
        new_name = new_name.replace("//", "_")
        new_name = new_name.replace("/", "_")
        new_name = new_name.replace("\\", "_")
        if new_name.endswith("_"):
            new_name = new_name[0:len(new_name)-1]
        if file_counter > 0:
            new_name += f"_{file_counter}.{file_extension}"
        else:
            new_name += f".{file_extension}"

        old_contents = ""
        if os.path.exists(output_dir + new_name):
            old_file_handle = open(output_dir + new_name)
            old_contents = old_file_handle.read()
            old_file_handle.close()

        if old_contents != new_file:
            print(f"-- -- -- stale file {new_name}")
            output_handle = open(output_dir + new_name, "w+")
            output_handle.write(new_file)
            output_handle.close()
        file_counter += 1

def process_excluded_file(excluded_name, output_dir):
    new_name = excluded_name.replace("/", "_")
    new_name = new_name.replace("//", "_")
    new_name = new_name.replace("\\", "_")
    new_name = "src_" + new_name

    old_contents = ""
    if os.path.exists(output_dir + new_name):
        old_file_handle = open(output_dir + new_name)
        old_contents = old_file_handle.read()
        old_file_handle.close()

    if old_contents != f"#include \"{excluded_name}\"":
        print(f"-- -- -- stale file {new_name}")
        output_handle = open(output_dir + new_name, "w+")
        output_handle.write(f"#include \"{excluded_name}\"")
        output_handle.close()

def should_skip_platform_folder(platform, root):
    if platform == "Linux":
        if "platform/windows" in root or "platform/mac" in root:
            return True
    elif platform == "Windows":
        if "platform/linux" in root or "platform\\linux" in root or "platform/mac" in root or "platform\\mac" in root:
            return True
    elif platform == "Mac":
        if "platform/linux" in root or "platform/windows" in root:
            return True

    return False

def create_unity_files(conf_path, platform):
    root_dir, excluded_files, output_dir = parse_conf_file(conf_path)

    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    for root, dirs, files in os.walk(root_dir):
        cpp_file_index = 0
        cpp_file_counter = 0
        objc_file_index = 0
        objc_file_counter = 0
        new_cpp_files = []
        new_objc_files = []

        if should_skip_platform_folder(platform, root):
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
                process_excluded_file(excluded_name, output_dir)
                continue

            if file.endswith(".cpp"):
                if cpp_file_index >= len(new_cpp_files):
                    new_cpp_files.append("")
                full_path = os.path.abspath(os.path.join(root, file))
                new_cpp_files[cpp_file_index] += "#include \"" + full_path + "\"\n"
                cpp_file_counter += 1
                if cpp_file_counter > 10:
                    cpp_file_index += 1
                    cpp_file_counter = 0

            if file.endswith(".mm"):
                if objc_file_index >= len(new_objc_files):
                    new_objc_files.append("")
                full_path = os.path.abspath(os.path.join(root, file))
                new_objc_files[objc_file_index] += "#include \"" + full_path + "\"\n"
                objc_file_counter += 1
                if objc_file_counter > 10:
                    objc_file_index += 1
                    objc_file_counter = 0

        write_output_files(new_cpp_files, "cpp", root, root_dir, output_dir)
        write_output_files(new_objc_files, "mm", root, root_dir, output_dir)

def main():
    conf_file = sys.argv[1]
    platform = sys.argv[2]
    print("-- -- Generating unity files...")
    create_unity_files(conf_file, platform)
    print("-- -- done.")

if __name__ == '__main__':
    main()