// blue noise table: contains the three tables distributed by Heitz.
// Offset 0: an Owen-scrambled Sobol sequence of 256 samples of 256 dimensions.
// Offset 65536: scrambling tile of 128x128 pixels; 128 * 128 * 8 values.
// Offset 65536 * 3: ranking tile of 128x128 pixels; 128 * 128 * 8 values. Total: 320KB.