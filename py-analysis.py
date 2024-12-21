#!/usr/bin/env python3

import os
import re
from typing import List, Dict
from dataclasses import dataclass
from datetime import datetime

@dataclass
class DissertationInfo:
    """Informações da dissertação"""
    title: str = "Modelos de Regressão Simbólica Através de Biased Random-Key Genetic Programming em Aplicações na Física"
    author: str = "Filipe Pessôa Sousa"
    advisors: List[str] = None
    institution: str = "UERJ - Instituto de Matemática e Estatística"
    program: str = "Mestrado em Ciências Computacionais"
    year: int = 2023
    abstract: str = None
    
    def __post_init__(self):
        self.advisors = ["Igor Machado Coelho", "Cristiane Oliveira de Faria"]
        self.abstract = """Este trabalho foca na aplicação do BRKGP em Regressão Simbólica para 
        descobrir funções físicas a partir de dados numéricos. Utiliza pilha para avaliação de 
        expressões e análise dimensional com GiNaC."""

@dataclass
class CodeStructure:
    """Estrutura de código fonte"""
    headers: List[str] = None
    classes: List[str] = None
    functions: List[str] = None
    includes: List[str] = None
    
    def __post_init__(self):
        self.headers = self.headers or []
        self.classes = self.classes or []
        self.functions = self.functions or []
        self.includes = self.includes or []

class ProjectAnalyzer:
    def __init__(self, root_dir: str):
        self.root_dir = root_dir
        self.output_file = "total-structure-report.txt"
        self.structures: Dict[str, CodeStructure] = {}
        self.dissertation = DissertationInfo()
        
        # Arquivos importantes que devem ser analisados
        self.key_files = {
            'BRKGA.hpp', 'Evaluator.hpp', 'PrintIO.hpp', 
            'ReadIO.hpp', 'Utils.hpp', 'main.cpp'
        }

    def safe_read_file(self, filepath: str) -> str:
        """Tenta ler arquivo com diferentes encodings"""
        encodings = ['utf-8', 'latin1', 'iso-8859-1', 'cp1252']
        for encoding in encodings:
            try:
                with open(filepath, 'r', encoding=encoding) as f:
                    return f.read()
            except UnicodeDecodeError:
                continue
            except Exception as e:
                print(f"Erro ao ler {filepath}: {str(e)}")
                return ""
        return ""

    def extract_file_info(self, filepath: str, content: str) -> CodeStructure:
        """Extrai informações relevantes do arquivo"""
        # Remove comentários
        content = re.sub(r'/\*[\s\S]*?\*/', '', content)
        content = re.sub(r'//[^\n]*', '', content)
        
        structure = CodeStructure()
        
        # Extrai includes relevantes
        includes = re.findall(r'#\s*include\s*[<"]([^>"]+)[>"]', content)
        structure.includes = [inc for inc in includes if any(x in inc for x in ['brkgp/', 'Scanner/'])]
        
        # Extrai classes importantes
        class_pattern = r'(class|struct)\s+(\w+)(?:\s*:\s*(?:public|protected|private)\s+[^{;]+)?'
        classes = re.finditer(class_pattern, content)
        structure.classes = [m.group(2) for m in classes if not any(x in m.group(2).lower() for x in ['test', 'mock'])]
        
        # Extrai funções principais
        function_pattern = r'(?:virtual\s+|static\s+)?(?:\w+\s+)?(\w+)\s*\([^)]*\)\s*(?:const|override)?'
        functions = re.finditer(function_pattern, content)
        
        filtered_functions = []
        for m in functions:
            func_name = m.group(1)
            # Ignora funções de teste/assert e funções muito comuns
            if not any(x in func_name.lower() for x in ['test', 'assert', 'print', 'get', 'set']):
                filtered_functions.append(func_name)
        
        structure.functions = list(set(filtered_functions))
        
        return structure

    def analyze_directory(self):
        """Analisa diretórios principais"""
        print(f"Analisando diretório {self.root_dir}...")
        
        # Diretórios importantes
        for root, dirs, files in os.walk(self.root_dir):
            # Ignora diretórios não relevantes
            if any(x in root for x in ['.git', 'bazel-', 'thirdparty']):
                continue
            
            for file in files:
                # Analisa apenas arquivos importantes
                if file in self.key_files or any(file.endswith(x) for x in ['.cpp', '.hpp', '.h']):
                    filepath = os.path.join(root, file)
                    if 'test' not in filepath and 'thirdparty' not in filepath:
                        content = self.safe_read_file(filepath)
                        if content:
                            self.structures[filepath] = self.extract_file_info(filepath, content)

    def generate_report(self):
        """Gera relatório do projeto"""
        with open(self.output_file, 'w', encoding='utf-8') as f:
            self._write_header(f)
            self._write_dissertation_info(f)
            self._write_technical_info(f)
            self._write_project_structure(f)
            self._write_build_info(f)
            print(f"Relatório gerado em {self.output_file}")
    
    def _write_header(self, f):
        f.write("# BRKGA-Modelos-de-Regressao\n\n")
        f.write(f"Análise gerada em: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
    
    def _write_dissertation_info(self, f):
        f.write("## Sobre o Projeto\n")
        f.write(f"Título: {self.dissertation.title}\n")
        f.write(f"Autor: {self.dissertation.author}\n")
        f.write(f"Orientadores: {', '.join(self.dissertation.advisors)}\n")
        f.write(f"Instituição: {self.dissertation.institution}\n")
        f.write(f"Programa: {self.dissertation.program}\n")
        f.write(f"Ano: {self.dissertation.year}\n\n")
        f.write("### Resumo\n")
        f.write(f"{self.dissertation.abstract}\n\n")
    
    def _write_technical_info(self, f):
        f.write("## Aspectos Técnicos\n")
        f.write("### Ambiente\n")
        f.write("- Linguagem: C++20\n")
        f.write("- Build System: Bazel 7.4.1\n")
        f.write("- Sistema: Ubuntu 22\n")
        
        f.write("\n### Componentes Principais\n")
        f.write("- Meta-heurística: BRKGP (Biased Random-Key Genetic Programming)\n")
        f.write("- Manipulação Simbólica: GiNaC\n")
        f.write("- Precisão Numérica: CLN & Kahan\n")
        f.write("- Paralelização: OpenMP\n")
        f.write("- Parser de Entrada: Scanner\n\n")
    
    def _write_project_structure(self, f):
        f.write("## Estrutura do Código\n")
        
        # Primeiro lista arquivos principais
        main_files = {k: v for k, v in self.structures.items() 
                     if any(x in k for x in ['src/main', 'include/brkgp'])}
        
        for filepath, structure in sorted(main_files.items()):
            if not structure.classes and not structure.functions:
                continue
                
            rel_path = os.path.relpath(filepath, self.root_dir)
            f.write(f"\n### {rel_path}\n")
            
            if structure.includes:
                f.write("\nDependências principais:\n")
                for inc in sorted(structure.includes):
                    f.write(f"- {inc}\n")
            
            if structure.classes:
                f.write("\nClasses/Estruturas:\n")
                for cls in sorted(structure.classes):
                    f.write(f"- {cls}\n")
            
            if structure.functions:
                f.write("\nFunções principais:\n")
                for func in sorted(structure.functions):
                    if len(func) < 50:  # Ignora funções muito longas
                        f.write(f"- {func}\n")
            
            f.write("\n")
    
    def _write_build_info(self, f):
        f.write("## Build e Execução\n")
        f.write("\n### Build (build.sh)\n```bash\n")
        f.write("# Configuração do compilador\n")
        f.write("export CC=gcc CXX=g++\n\n")
        f.write("# Flags de otimização\n")
        f.write("OPTS='-c opt --copt=-march=native --copt=-O3 --copt=-fopenmp'\n\n")
        f.write("# Build principal\n")
        f.write("bazel build $OPTS //src:brkga-regress\n```\n\n")
        
        f.write("### Execução (run.sh)\n```bash\n")
        f.write("BINARY='bazel-bin/src/brkga-regress'\n")
        f.write("ARGS='input.txt 30 100 25 10 85 100 100 80 1'\n")
        f.write("\n# Parâmetros:\n")
        f.write("#  - input.txt: arquivo de entrada\n")
        f.write("#  - 30: maxConst (número máximo de constantes)\n")
        f.write("#  - 100: populationLen (tamanho da população)\n")
        f.write("#  - 25: eliteSize (% elite)\n")
        f.write("#  - 10: mutantSize (% mutantes)\n")
        f.write("#  - 85: eliteBias (viés para elite)\n")
        f.write("#  - 100: restartMax (máx. reinícios)\n")
        f.write("#  - 100: noImprovementMax (gerações sem melhoria)\n")
        f.write("#  - 80: training (% dados para treino)\n")
        f.write("#  - 1: seed (semente aleatória)\n\n")
        f.write("$BINARY $ARGS\n```\n")

def main():
    analyzer = ProjectAnalyzer(".")
    analyzer.analyze_directory()
    analyzer.generate_report()

if __name__ == "__main__":
    main()