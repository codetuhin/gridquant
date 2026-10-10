# GridQuant: C++ Continuous Double Auction Engine

GridQuant is a high-performance matching engine engineered for decentralized Peer-to-Peer (P2P) energy microgrids. Built in C++, it bypasses traditional utility brokers to enable sub-second energy trading between residential solar prosumers and electric vehicles (EVs).

## ⚙️ Core Architecture
* **Algorithmic Matching:** Utilizes C++ STL priority queues (`evbids` and `solarask`) to process limit orders with O(log N) time complexity.
* **Low-Latency Finality:** Bypasses legacy networks to achieve transaction settlement finality within a 70–116 ms latency window.
* **Algorithmic Trust System:** Features a credibility-scoring algorithm that monitors historical execution reliability to dynamically adjust settlement pricing.

## 📂 Testing Modules
* `Test_CoreMatchingEngine.cpp` - Validates the primary double auction loop and mid-market price execution.
* `Test_CredibilityAlgorithm.cpp` - Tests the historical contract tracking and node scoring mechanisms.
* `Test_LedgerAndBalances.cpp` - Simulates token settlement and automated escrow functions.
* `Test_TelemetryLogic.cpp` - Tests the conversion of raw virtual allocations into tradable surplus.
