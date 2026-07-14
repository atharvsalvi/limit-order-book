#pragma once
using namespace std;

class PnLTracker {
	public:
		void processFill();
		void markToMarket(double last_price);

		double getRealizedPnL()   const { return realized_pnl; }
		double getUnrealizedPnL() const { return unrealized_pnl; }
		double getTotalPnL()      const { return realized_pnl + unrealized_pnl; }
		int    getNetPosition()   const { return net_position; }
		double getAvgEntryPrice() const { return avg_entry_price; }

	private:
		int net_position = 0;
		double avg_entry_price = 0.0;
		double realized_pnl = 0.0;
		double unrealized_pnl = 0.0;
};

extern PnLTracker tracker;