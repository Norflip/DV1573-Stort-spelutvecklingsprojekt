#include "stdafx.h"
#include "Pathfinding.h"

Pathfinding::Pathfinding(PlayerComp* player)
	: cols(32), rows(32), player(player), pathFound(false), playerRadius(2.5)
{
	timer.Start();
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			grid[x][y] = nullptr;
		}
	}
}

Pathfinding::~Pathfinding()
{
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			delete grid[x][y];
		}
	}
}

void Pathfinding::Initialize()
{
	this->enemyRB = GetOwner()->GetComponent<RigidBodyComponent>();
	this->enemyStatsComp = GetOwner()->GetComponent<EnemyStatsComp>();
	for (int x = 0; x < this->cols; x++)
	{
		for (int y = 0; y < this->rows; y++)
		{
			this->grid[x][y] = new Node();
		}
	}
}

void Pathfinding::Update(const float& deltaTime)
{
	timer.Update();
	ResetPath();
	AStar();

	dx::XMFLOAT3 enemyPos;
	dx::XMStoreFloat3(&enemyPos, GetOwner()->GetTransform().GetPosition());
	if (pathFound)
	{
		FollowPath();
	}
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			dx::XMFLOAT3 color;
			if (grid[i][j]->obstacle)
			{
				color = dx::XMFLOAT3(0, 0, 0);
			}
			else
			{
				if (grid[i][j]->openSet)
					color = dx::XMFLOAT3(0, 1, 0);
				else if (grid[i][j]->correctPath)
					color = dx::XMFLOAT3(0, 0, 1);
				else if (grid[i][j]->closedSet)
					color = dx::XMFLOAT3(1, 0, 0);
				else
					color = dx::XMFLOAT3(1, 1, 1);
			}
			box.DrawBox(dx::XMFLOAT3(grid[i][j]->pos.x + (int)enemyPos.x - (cols/2), enemyPos.y + 2, grid[i][j]->pos.y + (int)enemyPos.z - (rows/2)), dx::XMFLOAT3(0.8, 0.8, 0.8), color);
		}
	}
}

void Pathfinding::AStar()
{
	timer.Restart();
	dx::XMFLOAT3 enemyGridPos;
	dx::XMStoreFloat3(&enemyGridPos, GetOwner()->GetTransform().GetPosition());
	dx::XMFLOAT3 playerGridPos;
	dx::XMStoreFloat3(&playerGridPos, player->GetOwner()->GetTransform().GetPosition());
	playerGridPos.x = (abs((int)playerGridPos.x) - abs((int)enemyGridPos.x)) + (cols / 2);
	playerGridPos.z = (abs((int)playerGridPos.z) - abs((int)enemyGridPos.z)) + (rows / 2);
	enemyGridPos.x = (abs((int)enemyGridPos.x) - abs((int)enemyGridPos.x)) + (cols / 2);
	enemyGridPos.z = (abs((int)enemyGridPos.z) - abs((int)enemyGridPos.z)) + (rows / 2);

	if ((int)playerGridPos.x >= 0 && (int)playerGridPos.z >= 0 && 
		(int)playerGridPos.x < (cols-1) && (int)playerGridPos.z < (rows - 1))
	{
		Node* start = grid[(int)enemyGridPos.x][(int)enemyGridPos.z];
		Node* end = grid[(int)playerGridPos.x][(int)playerGridPos.z];

		//Node* start = grid[0][0];
		//Node* end = grid[cols - 1][rows - 1];
		start->obstacle = false;
		end->obstacle = false;
		std::vector<Node*> openSet;
		std::vector<Node*> closedSet;
		openSet.push_back(start);
		grid[(int)start->pos.x][(int)start->pos.y]->openSet = true;
		while (openSet.size() > 0)
		{
			Node* current = nullptr;

			std::make_heap(openSet.begin(), openSet.end(), Compare());
			current = openSet[0];
			openSet.erase(openSet.begin());

			if (current != nullptr && current->pos.x == end->pos.x && current->pos.y == end->pos.y)
			{
				pathFound = true;
				Node* temp = current;
				grid[(int)temp->pos.x][(int)temp->pos.y]->correctPath = true;
				correctPath.push_back(grid[(int)temp->pos.x][(int)temp->pos.y]);
				while (temp->previous != nullptr)
				{
					temp = temp->previous;
					grid[(int)temp->pos.x][(int)temp->pos.y]->correctPath = true;
					correctPath.push_back(grid[(int)temp->pos.x][(int)temp->pos.y]);
				}
			}

			openSet.shrink_to_fit();
			grid[(int)current->pos.x][(int)current->pos.y]->openSet = false;
			closedSet.push_back(current);
			grid[(int)current->pos.x][(int)current->pos.y]->closedSet = true;

			for (int i = 0; i < current->neighbors.size(); i++)
			{
				if ((closedSet.empty() || std::find(closedSet.begin(), closedSet.end(), current->neighbors[i]) == closedSet.end())
					&& !current->neighbors[i]->obstacle)
				{
					int tempG = current->gCost + 1;

					bool newPath = false;
					if (std::find(openSet.begin(), openSet.end(), current->neighbors[i]) != openSet.end())
					{
						if (tempG < current->neighbors[i]->gCost)
						{
							current->neighbors[i]->gCost = tempG;
							newPath = true;
						}
					}
					else
					{
						current->neighbors[i]->gCost = tempG;
						openSet.push_back(current->neighbors[i]);
						grid[(int)current->neighbors[i]->pos.x][(int)current->neighbors[i]->pos.y]->openSet = true;
						newPath = true;
					}
					if (newPath)
					{
						current->neighbors[i]->hCost = GetDistance(current->neighbors[i], end);
						current->neighbors[i]->fCost = current->neighbors[i]->gCost + current->neighbors[i]->hCost;
						current->neighbors[i]->previous = current;
					}
				}
			}
		}
		if (!pathFound)
			std::cout << "No path available" << std::endl;
	}
}

void Pathfinding::AddNeighbors(Node* node)
{
	if (node->pos.x < (cols - 1))
		node->neighbors.push_back(grid[(int)node->pos.x + 1][(int)node->pos.y]);
	if (node->pos.x > 0)
		node->neighbors.push_back(grid[(int)node->pos.x - 1][(int)node->pos.y]);
	if (node->pos.y < (rows - 1))
		node->neighbors.push_back(grid[(int)node->pos.x][(int)node->pos.y + 1]);
	if (node->pos.y > 0)
		node->neighbors.push_back(grid[(int)node->pos.x][(int)node->pos.y - 1]);
	if (node->pos.x > 0 && node->pos.y > 0)
		node->neighbors.push_back(grid[(int)node->pos.x - 1][(int)node->pos.y - 1]);
	if (node->pos.x < cols - 1 && node->pos.y > 0)
		node->neighbors.push_back(grid[(int)node->pos.x + 1][(int)node->pos.y - 1]);
	if (node->pos.x > 0 && node->pos.y < rows - 1)
		node->neighbors.push_back(grid[(int)node->pos.x - 1][(int)node->pos.y + 1]);
	if (node->pos.x < cols - 1 && node->pos.y < rows - 1)
		node->neighbors.push_back(grid[(int)node->pos.x + 1][(int)node->pos.y + 1]);
}

void Pathfinding::AddNeighbors2(Node* node)
{
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			if (x == 0 && y == 0)
				continue;

			int checkX = (int)node->pos.x + x;
			int checkY = (int)node->pos.y + y;

			if (checkX >= 0 && checkX < ((cols -1) * (rows - 1)) && checkY >= 0 && checkY < ((cols - 1) * (rows - 1)))
			{
				node->neighbors.push_back(grid[checkX][checkY]);
			}
		}
	}
}

int Pathfinding::GetDistance(Node* nodeA, Node* nodeB)
{
	return abs(nodeA->pos.x - nodeB->pos.x) + abs(nodeA->pos.y - nodeB->pos.y);
}

void Pathfinding::AddObstacles()
{
	srand(time(NULL));
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			int randNr = rand() % 100;
			if(randNr < 30)
				grid[x][y]->obstacle = true;
		}
	}
}

void Pathfinding::ResetPath()
{
	pathFound = false;
	correctPath.clear();
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			grid[x][y]->pos = dx::XMFLOAT2(x, y);
			grid[x][y]->gCost = 0;
			grid[x][y]->hCost = 0;
			grid[x][y]->fCost = 0;
			grid[x][y]->obstacle = false;
			grid[x][y]->openSet = false;
			grid[x][y]->closedSet = false;
			grid[x][y]->correctPath = false;
			grid[x][y]->previous = nullptr;
			grid[x][y]->neighbors.clear();
		}
	}

	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			AddNeighbors(grid[x][y]);
		}
	}

	AddObstacles();
}

void Pathfinding::FollowPath()
{
	if (correctPath.size() > 1)
	{
		dx::XMFLOAT3 enemyNewPos;
		dx::XMStoreFloat3(&enemyNewPos, GetOwner()->GetTransform().GetPosition());

		Node* nodeA = correctPath[correctPath.size() - 1];
		correctPath.erase(correctPath.begin() + (correctPath.size() - 1));
		correctPath.shrink_to_fit();
		Node* nodeB = correctPath[correctPath.size() - 1];
		correctPath.erase(correctPath.begin() + (correctPath.size() - 1));
		correctPath.shrink_to_fit();

		dx::XMFLOAT2 walkTo = { nodeA->pos.x - nodeB->pos.x, nodeA->pos.y - nodeB->pos.y };
		enemyNewPos.x = walkTo.x;
		enemyNewPos.z = walkTo.y;
		std::cout << "enemyNewPos x: " << enemyNewPos.x << " z: " << enemyNewPos.z << std::endl;
		dx::XMVECTOR enemyNewPosVec = dx::XMVectorSet(enemyNewPos.x, 0, enemyNewPos.y, 0);

		dx::XMFLOAT3 moveDir = { 0.0f, 0.0f, 0.0f };
		dx::XMVECTOR moveVec = dx::XMVectorAdd(enemyNewPosVec, GetOwner()->GetTransform().GetPosition());
		dx::XMVECTOR normVec = dx::XMVector3Normalize(moveVec);
		dx::XMFLOAT3 normDir;
		dx::XMStoreFloat3(&normDir, normVec);

		dx::XMVECTOR lenVec = dx::XMVector3Length(moveVec);
		float length;
		dx::XMStoreFloat(&length, lenVec);

		if (length >= playerRadius)
		{
			moveDir.x = normDir.x;
			moveDir.z = normDir.z;
		}

		dx::XMFLOAT3 vel = enemyRB->GetLinearVelocity();

		dx::XMFLOAT3 move = { moveDir.x * enemyStatsComp->GetSpeed(), vel.y, moveDir.z * enemyStatsComp->GetSpeed() };

		enemyRB->SetLinearVelocity(move);
		dx::XMVECTOR enemyPos = GetOwner()->GetTransform().GetPosition();

		dx::XMVECTOR dirVec = dx::XMVector3Normalize(dx::XMVectorSubtract(GetOwner()->GetTransform().GetPosition(), moveVec));
		dx::XMFLOAT3 dirToNode;
		dx::XMStoreFloat3(&dirToNode, dirVec);
		float angle = 180.f + atan2f(dirToNode.x, dirToNode.z) * (180.f / Math::PI);
		float rotation = angle * Math::ToRadians;
		dx::XMVECTOR right = GetOwner()->GetTransform().TransformDirection({ 1,0,0 });
		dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, rotation));
		GetOwner()->GetTransform().SetRotation(eulerRotation);
		enemyRB->SetRotation(eulerRotation);
	}
}
