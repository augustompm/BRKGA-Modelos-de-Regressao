#!/usr/bin/env python3
import sys
import subprocess
import re
import os

class Logger:
    def __init__(self, filename):
        self.file = open(filename, "a", buffering=1)
    
    def log(self, msg):
        """Log message only to file"""
        self.file.write(f"{msg}\n")
        self.file.flush()
    
    def close(self):
        if self.file:
            self.file.close()

def get_param_value(param_dict, key, default_value):
    """Helper to get parameter value with default"""
    if key in param_dict:
        return param_dict[key]
    return str(default_value)

def extract_best_value(stdout, stderr):
    """Extract the best value from output, trying different patterns"""
    # Try to find "best=X" pattern in either stdout or stderr
    for output in [stdout, stderr]:
        # Try the explicit "best=X" pattern
        matches = re.findall(r"best\s*=\s*([+-]?\d+(\.\d+)?)", output)
        if matches:
            return float(matches[-1][0])
        
        # As backup, try to find just a number at the start of a line
        matches = re.findall(r"^\s*([+-]?\d+(\.\d+)?)\s*$", output, re.MULTILINE)
        if matches:
            return float(matches[-1][0])
    
    return None

def main():
    # Silence all stderr by default
    if not os.environ.get("IRACE_DEBUG"):
        sys.stderr = open(os.devnull, 'w')
    
    # Setup logging to file only
    logger = Logger("irace-run1.txt")
    
    try:
        args = sys.argv[1:]
        if len(args) < 4:
            print("999999")  # Only output number for irace
            logger.log("[ERROR] Incorrect runner usage. Expected args >= 4.")
            sys.exit(1)

        # Parse arguments
        id_config = args[0]
        id_instance = args[1]
        seed = args[2]
        instance = args[3]
        params_from_irace = args[4:]

        # Log the received arguments for debugging
        logger.log(f"[ARGS] Full args received: {' '.join(args)}")

        # Process instance path
        instance_path = instance if instance.startswith("/") else os.path.join("running_instances", instance)
        logger.log(f"[PATH] Using instance path: {instance_path}")

        # Process parameters with defaults matching C++ program
        param_dict = {}
        i = 0
        while i < len(params_from_irace):
            key = params_from_irace[i].lstrip('-')
            val = params_from_irace[i + 1]
            param_dict[key] = val
            i += 2

        # Default values matching C++ program
        defaults = {
            "populationLen": "100",
            "eliteSize": "25",
            "mutantSize": "10",
            "eliteBias": "85",
            "iterationMax": "50",
            "noImprovementMax": "50",
            "moreDiversity": "20"
        }

        # Build command with parameters in correct order
        cmd = [
            "./bazel-bin/app_demo",
            seed,                                                   # seed (argv[1])
            instance_path,                                         # instance path (argv[2])
            get_param_value(param_dict, "populationLen", defaults["populationLen"]),       # argv[3]
            get_param_value(param_dict, "eliteSize", defaults["eliteSize"]),               # argv[4]
            get_param_value(param_dict, "mutantSize", defaults["mutantSize"]),             # argv[5]
            get_param_value(param_dict, "eliteBias", defaults["eliteBias"]),               # argv[6]
            get_param_value(param_dict, "iterationMax", defaults["iterationMax"]),         # argv[7]
            get_param_value(param_dict, "noImprovementMax", defaults["noImprovementMax"]), # argv[8]
            "5",                                                   # reset (argv[9])
            get_param_value(param_dict, "moreDiversity", defaults["moreDiversity"])        # training (argv[10])
        ]
        
        logger.log(f"[EXEC] Command to execute: {' '.join(cmd)}")

        # Run command and capture output
        process = subprocess.run(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=5000
        )

        # Log full output to file
        logger.log("\n=== STDOUT ===\n" + process.stdout)
        logger.log("\n=== STDERR ===\n" + process.stderr)

        if process.returncode != 0:
            print("999999")  # Error value for irace
            logger.log(f"[ERROR] Process returned code {process.returncode}")
            sys.exit(1)

        # Try to find best value in output
        best_value = extract_best_value(process.stdout, process.stderr)
        
        if best_value is not None:
            print(f"{best_value:.6f}")
            logger.log(f"[SUCCESS] Returned value: {best_value:.6f}")
            sys.exit(0)

        print("999999")  # Error value for irace
        logger.log("[ERROR] No valid best value found in output")
        sys.exit(1)

    except Exception as e:
        print("999999")  # Error value for irace
        logger.log(f"[ERROR] Exception occurred: {str(e)}")
        sys.exit(1)
    finally:
        logger.close()

if __name__ == "__main__":
    main()