#!/usr/bin/env python3
"""
benchmark_compare.py — Compare benchmark results against a stored baseline.

Reads a current benchmark JSON (produced by spatialrender_benchmark) and a
baseline JSON (committed to benchmarks/baseline/), computes the regression
percentage for avg_fps and avg_frame_time_us, and exits with code 1 if any
metric exceeds the configured threshold.

Usage:
    python3 tools/benchmark_compare.py \
        --current  build/benchmarks/benchmark_100_objects.json \
        --baseline benchmarks/baseline/benchmark_100_objects.json \
        --threshold 10.0

    # Compare all results in a directory against all baselines:
    python3 tools/benchmark_compare.py \
        --current-dir  build/benchmarks \
        --baseline-dir benchmarks/baseline \
        --threshold 10.0
"""

import argparse
import json
import os
import sys
from pathlib import Path


# Metrics where LOWER is better (regression = value increased)
LOWER_IS_BETTER = {"avg_frame_time_us", "avg_render_time_us", "frame_variance"}

# Metrics where HIGHER is better (regression = value decreased)
HIGHER_IS_BETTER = {"avg_fps"}


def load_json(path: Path) -> dict:
    with open(path) as f:
        return json.load(f)


def compute_regression(baseline_val: float, current_val: float, metric: str) -> float:
    """
    Return the regression percentage (positive = regression, negative = improvement).
    For lower-is-better metrics: regression = (current - baseline) / baseline * 100
    For higher-is-better metrics: regression = (baseline - current) / baseline * 100
    """
    if baseline_val == 0:
        return 0.0
    if metric in LOWER_IS_BETTER:
        return (current_val - baseline_val) / baseline_val * 100.0
    else:
        return (baseline_val - current_val) / baseline_val * 100.0


def compare_single(current_path: Path, baseline_path: Path, threshold: float) -> bool:
    """
    Compare one current result against one baseline.
    Returns True if all metrics are within threshold, False if any regression detected.
    """
    if not baseline_path.exists():
        print(f"  SKIP: No baseline found at {baseline_path}")
        return True

    current = load_json(current_path)
    baseline = load_json(baseline_path)

    complexity = current.get("scene_complexity", "?")
    print(f"\n── Scene complexity: {complexity} objects ──")
    print(f"   Baseline : {baseline_path}")
    print(f"   Current  : {current_path}")

    all_pass = True
    metrics_to_check = list(LOWER_IS_BETTER | HIGHER_IS_BETTER)

    for metric in sorted(metrics_to_check):
        if metric not in current or metric not in baseline:
            continue

        baseline_val = float(baseline[metric])
        current_val = float(current[metric])
        regression_pct = compute_regression(baseline_val, current_val, metric)

        direction = "▲" if regression_pct > 0 else "▼"
        status = "PASS"

        if regression_pct > threshold:
            status = "FAIL"
            all_pass = False

        print(
            f"   [{status}] {metric:<25} "
            f"baseline={baseline_val:>10.2f}  "
            f"current={current_val:>10.2f}  "
            f"regression={direction}{abs(regression_pct):.1f}%"
            + (f"  ← exceeds {threshold}% threshold" if status == "FAIL" else "")
        )

    return all_pass


def compare_directories(current_dir: Path, baseline_dir: Path, threshold: float) -> bool:
    """
    Compare all JSON files in current_dir against matching files in baseline_dir.
    """
    current_files = sorted(current_dir.glob("benchmark_*.json"))

    if not current_files:
        print(f"No benchmark result files found in {current_dir}")
        return True

    all_pass = True
    for current_path in current_files:
        baseline_path = baseline_dir / current_path.name
        if not compare_single(current_path, baseline_path, threshold):
            all_pass = False

    return all_pass


def main():
    parser = argparse.ArgumentParser(
        description="Compare benchmark results against a stored baseline."
    )

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--current", type=Path, help="Path to a single current result JSON")
    group.add_argument(
        "--current-dir", type=Path, help="Directory containing current result JSON files"
    )

    parser.add_argument("--baseline", type=Path, help="Path to a single baseline JSON")
    parser.add_argument(
        "--baseline-dir",
        type=Path,
        default=Path("benchmarks/baseline"),
        help="Directory containing baseline JSON files (default: benchmarks/baseline)",
    )

    parser.add_argument(
        "--threshold",
        type=float,
        default=10.0,
        help="Regression threshold in percent (default: 10.0)",
    )

    args = parser.parse_args()

    print(f"Benchmark regression check — threshold: {args.threshold}%")
    print("=" * 60)

    if args.current:
        if args.baseline is None:
            # Infer baseline path from current filename
            args.baseline = args.baseline_dir / args.current.name
        passed = compare_single(args.current, args.baseline, args.threshold)
    else:
        passed = compare_directories(args.current_dir, args.baseline_dir, args.threshold)

    print("\n" + "=" * 60)
    if passed:
        print("Result: PASS — no regressions detected")
        sys.exit(0)
    else:
        print(f"Result: FAIL — one or more metrics regressed beyond {args.threshold}%")
        sys.exit(1)


if __name__ == "__main__":
    main()
