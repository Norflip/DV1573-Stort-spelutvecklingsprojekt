#pragma once

class NodeClass
{
	public:
		NodeClass(int x, int y);
		~NodeClass() {}
		void SetX(int x) { this->x = x; }
		void SetY(int y) { this->y = y; }
		void SetGCost(int gCost) { this->gCost = gCost; }
		void SetHCost(int hCost) { this->hCost = hCost; }
		void SetFCost(int fCost) { this->fCost = fCost; }
		void SetOpenSet(bool open) { this->openSet = open; }
		void SetClosedSet(bool closed) { this->closedSet = closed; }
		void SetCorrectPath(bool correct) { this->closedSet = false; this->correctPath = correct; }
		void SetPrevious(NodeClass* prev) { this->previous = prev; }
		void AddNeighbor(NodeClass* neighbor) { this->neighbors.push_back(neighbor); }
		int GetX() const { return this->x; }
		int GetY() const { return this->y; }
		int GetGCost() const { return this->gCost; }
		int GetHCost() const { return this->hCost; }
		int GetFCost() const { return this->fCost; }
		bool GetOpenSet() const { return this->openSet; }
		bool GetClosedSet() const { return this->closedSet; }
		bool GetCorrectPath() const { return this->correctPath; }
		NodeClass* GetPrevious() const { return this->previous; }
		std::vector<NodeClass*> GetNeighbors() { return neighbors; }

	private:
		int x;
		int y;
		int gCost;
		int hCost;
		int fCost;
		bool openSet;
		bool closedSet;
		bool correctPath;
		NodeClass* previous;
		std::vector<NodeClass*> neighbors;
};