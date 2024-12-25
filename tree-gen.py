import os

base_path = os.getcwd()
output_file = "tree.txt"
ignored_dirs = {"tools", "thirdparty", "feynman-data"}
ignored_prefixes = ("bazel-",)

def write_tree(path, level=0, file=None):
    """
    Função recursiva para varrer a estrutura de diretórios e arquivos
    """
    indent = "    " * level 
    base_name = os.path.basename(path)

    if os.path.isdir(path):
        if base_name in ignored_dirs or base_name.startswith(ignored_prefixes):
            return
        file.write(f"{indent}{base_name}/\n")
        for entry in sorted(os.listdir(path)):
            full_path = os.path.join(path, entry)
            write_tree(full_path, level + 1, file)
    elif base_name.endswith(('.cpp', '.hpp')):
        file.write(f"{indent}{base_name}\n")

def include_file_contents(path, file):
    """
    Inclui o conteúdo de arquivos específicos no arquivo de saída
    """
    if os.path.exists(path):
        file.write(f"\nConteúdo de {os.path.basename(path)}:\n")
        with open(path, "r") as f:
            file.write(f.read())
        file.write("\n")

def generate_tree(base_path, output_file):
    """
    Gera a estrutura de diretórios e arquivos em um arquivo txt
    """
    with open(output_file, "w") as file:
        file.write(f"Estrutura do projeto em {base_path}\n\n")
        write_tree(base_path, file=file)
        include_file_contents(os.path.join(base_path, "run.sh"), file)
        include_file_contents(os.path.join(base_path, "build.sh"), file)

if __name__ == "__main__":
    print(f"Gerando estrutura de diretórios do projeto a partir de {base_path}...")
    generate_tree(base_path, output_file)
    print(f"Estrutura salva no arquivo: {output_file}")
