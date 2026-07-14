from enum import Enum
import pandas as pd
import math

# Define the decision states matching your C++ enums
class Decision(Enum):
    HOLD = 0
    BUY = 1
    SELL = 2

history = [1,1,1,1,1,1,1,1,1,1,1,1,1]
period = 10
num_std_dev = 2.0


def generate_signal(current_price: float) -> Decision:
    """Calculates Bollinger Bands and outputs a trading decision."""
    # We need enough historical data to calculate a valid variance
    if len(history) < period:
        return Decision.HOLD

    # 1. Calculate Simple Moving Average (SMA)
    sma = sum(history) / period

    print(sma)

    # 2. Calculate Standard Deviation
    variance_sum = sum((p - sma) ** 2 for p in history)
    std_dev = math.sqrt(variance_sum / period)

    # 3. Calculate Bands
    upper_band = sma + (num_std_dev * std_dev)
    lower_band = sma - (num_std_dev * std_dev)

    # 4. Generate Mean Reversion Signals
    if current_price >= upper_band:
        return Decision.SELL
    elif current_price <= lower_band:
        return Decision.BUY

    return Decision.HOLD
    
generate_signal(1.0)