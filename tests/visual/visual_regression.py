#!/usr/bin/env python3
"""
Visual Regression Testing for SpatialRender

Compares rendered images against golden references with configurable thresholds.
Generates heatmaps for differences and uploads artifacts on failure.
"""

import os
import sys
import argparse
import json
from pathlib import Path
from typing import Tuple, Dict, Optional
import numpy as np
from PIL import Image, ImageDraw, ImageFont


class VisualRegressionTester:
    def __init__(self, output_dir: str, golden_dir: str, threshold: float = 0.01):
        self.output_dir = Path(output_dir)
        self.golden_dir = Path(golden_dir)
        self.threshold = threshold
        self.results = []

        # Create directories
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.golden_dir.mkdir(parents=True, exist_ok=True)
        self.diff_dir = self.output_dir.parent / "diffs"
        self.diff_dir.mkdir(exist_ok=True)

    def compare_images(
        self, output_path: Path, golden_path: Path
    ) -> Tuple[bool, float, Optional[np.ndarray]]:
        """
        Compare two images pixel-by-pixel

        Returns: (match, difference_percentage, diff_image)
        """
        if not output_path.exists():
            return False, 1.0, None

        if not golden_path.exists():
            print(f"Warning: Golden image not found: {golden_path}")
            return False, 1.0, None

        # Load images
        output_img = Image.open(output_path).convert("RGB")
        golden_img = Image.open(golden_path).convert("RGB")

        # Resize if dimensions don't match
        if output_img.size != golden_img.size:
            print(
                f"Warning: Image size mismatch. Output: {output_img.size}, Golden: {golden_img.size}"
            )
            golden_img = golden_img.resize(output_img.size, Image.Resampling.LANCZOS)

        # Convert to numpy arrays
        output_arr = np.array(output_img).astype(np.float32)
        golden_arr = np.array(golden_img).astype(np.float32)

        # Compute difference
        diff_arr = np.abs(output_arr - golden_arr)
        diff_percentage = np.mean(diff_arr) / 255.0

        # Create heatmap
        diff_normalized = (diff_arr / 255.0 * 255).astype(np.uint8)
        diff_image = Image.fromarray(diff_normalized)

        match = diff_percentage <= self.threshold
        return match, diff_percentage, diff_image

    def test_image(self, test_name: str) -> bool:
        """Test a single image against its golden reference"""
        output_path = self.output_dir / f"{test_name}.png"
        golden_path = self.golden_dir / f"{test_name}.png"

        match, diff_pct, diff_img = self.compare_images(output_path, golden_path)

        result = {
            "name": test_name,
            "match": match,
            "difference": diff_pct,
            "output_path": str(output_path),
            "golden_path": str(golden_path) if golden_path.exists() else None,
        }

        if not match:
            # Save difference heatmap
            diff_path = self.diff_dir / f"{test_name}_diff.png"
            if diff_img:
                diff_img.save(diff_path)
                result["diff_path"] = str(diff_path)

            print(f"FAIL: {test_name} - Difference: {diff_pct:.4f} (threshold: {self.threshold})")
        else:
            print(f"PASS: {test_name} - Difference: {diff_pct:.4f}")

        self.results.append(result)
        return match

    def generate_report(self, report_path: Path):
        """Generate JSON report of test results"""
        report = {
            "total_tests": len(self.results),
            "passed": sum(1 for r in self.results if r["match"]),
            "failed": sum(1 for r in self.results if not r["match"]),
            "threshold": self.threshold,
            "results": self.results,
        }

        with open(report_path, "w") as f:
            json.dump(report, f, indent=2)

        print(f"\nReport generated: {report_path}")
        print(
            f"Total: {report['total_tests']}, Passed: {report['passed']}, Failed: {report['failed']}"
        )

    def run_all_tests(self) -> bool:
        """Run all visual regression tests"""
        if not self.output_dir.exists():
            print(f"Error: Output directory not found: {self.output_dir}")
            return False

        # Find all test images
        test_images = list(self.output_dir.glob("*.png"))

        if not test_images:
            print("No test images found")
            return False

        all_passed = True
        for img_path in test_images:
            test_name = img_path.stem
            if not self.test_image(test_name):
                all_passed = False

        # Generate report
        report_path = self.output_dir.parent / "visual_regression_report.json"
        self.generate_report(report_path)

        return all_passed


def main():
    parser = argparse.ArgumentParser(description="Visual regression testing for SpatialRender")
    parser.add_argument(
        "--output-dir",
        default="tests/visual/output",
        help="Directory containing test output images",
    )
    parser.add_argument(
        "--golden-dir",
        default="tests/visual/golden",
        help="Directory containing golden reference images",
    )
    parser.add_argument(
        "--threshold", type=float, default=0.01, help="Difference threshold (0.0-1.0)"
    )
    parser.add_argument("--test", help="Test a specific image (without extension)")

    args = parser.parse_args()

    tester = VisualRegressionTester(args.output_dir, args.golden_dir, args.threshold)

    if args.test:
        success = tester.test_image(args.test)
    else:
        success = tester.run_all_tests()

    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
