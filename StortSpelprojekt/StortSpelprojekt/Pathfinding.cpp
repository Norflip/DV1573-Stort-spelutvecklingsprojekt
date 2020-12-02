#include "stdafx.h"
#include "Pathfinding.h"
#include "Engine.h"

Pathfinding::Pathfinding(PlayerComp* player)
	: cols(32), rows(32), player(player), pathFound(false), playerRadius(2.5), length(10.0f)
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
	dx::XMFLOAT3 enemyGridPos;
	dx::XMStoreFloat3(&enemyGridPos, GetOwner()->GetTransform().GetPosition());
	dx::XMFLOAT3 playerGridPos;
	dx::XMStoreFloat3(&playerGridPos, player->GetOwner()->GetTransform().GetPosition());
	playerGridPos.x = (abs((int)playerGridPos.x) - abs((int)enemyGridPos.x)) + (cols / 2);
	playerGridPos.z = (abs((int)playerGridPos.z) - abs((int)enemyGridPos.z)) + (rows / 2);

	if (timer.GetSeconds() > 2.0f)
	{
		ResetPath();
		AStar();
		timer.Restart();
	}
	if (pathFound)
	{
		FollowPath();
	}

	//dx::XMFLOAT3 enemyPos;
	//dx::XMStoreFloat3(&enemyPos, GetOwner()->GetTransform().GetPosition());

	//for (int i = 0; i < cols; i++)
	//{
	//	for (int j = 0; j < rows; j++)
	//	{
	//		dx::XMFLOAT3 color;
	//		if (grid[i][j]->obstacle)
	//		{
	//			color = dx::XMFLOAT3(0, 0, 0);
	//		}
	//		else
	//		{
	//			if (grid[i][j]->openSet)
	//				color = dx::XMFLOAT3(0, 1, 0);
	//			else if (grid[i][j]->correctPath)
	//				color = dx::XMFLOAT3(0, 0, 1);
	//			else if (grid[i][j]->closedSet)
	//				color = dx::XMFLOAT3(1, 0, 0);
	//			else
	//				color = dx::XMFLOAT3(1, 1, 1);
	//		}
	//		DShape::DrawBox(dx::XMFLOAT3(grid[i][j]->pos.x + (int)enemyPos.x - (cols/2), enemyPos.y + 7, grid[i][j]->pos.y + (int)enemyPos.z - (rows/2)), dx::XMFLOAT3(0.8, 0.8, 0.8), color);
	//	}
	//}
}

void Pathfinding::SetPlayer(PlayerComp* playerComp)
{
	player = playerComp;
}

void Pathfinding::AStar()
{
	dx::XMFLOAT3 enemyGridPos;
	dx::XMStoreFloat3(&enemyGridPos, GetOwner()->GetTransform().GetPosition());
	dx::XMFLOAT3 playerGridPos;
	dx::XMStoreFloat3(&playerGridPos, player->GetOwner()->GetTransform().GetPosition());
	playerGridPos.x = (abs((int)playerGridPos.x) - abs((int)enemyGridPos.x)) + (cols / 2);		//player worldpos to gridPos
	playerGridPos.z = (abs((int)playerGridPos.z) - abs((int)enemyGridPos.z)) + (rows / 2);
	enemyGridPos.x = (abs((int)enemyGridPos.x) - abs((int)enemyGridPos.x)) + (cols / 2);		//enemy worldpos to gridCenterPos
	enemyGridPos.z = (abs((int)enemyGridPos.z) - abs((int)enemyGridPos.z)) + (rows / 2);

	if ((int)playerGridPos.x >= 0 && (int)playerGridPos.z >= 0 && 
		(int)playerGridPos.x < (cols-1) && (int)playerGridPos.z < (rows - 1))
	{
		Node* start = grid[(int)enemyGridPos.x][(int)enemyGridPos.z];							//start node is center node of grid
		Node* end = grid[(int)playerGridPos.x][(int)playerGridPos.z];							//player pos in grid

		start->obstacle = false;
		end->obstacle = false;
		std::vector<Node*> openSet;
		std::vector<Node*> closedSet;
		openSet.push_back(start);										
		grid[(int)start->pos.x][(int)start->pos.y]->openSet = true;
		while (openSet.size() > 0 || !pathFound)
		{
			Node* current = nullptr;

			std::make_heap(openSet.begin(), openSet.end(), Compare());			//sort openSet
			current = openSet[0];
			openSet.erase(openSet.begin());

			if (current != nullptr && current->pos.x == end->pos.x && current->pos.y == end->pos.y)		//if current is end, save the correct path and end loop
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
			else
			{
				openSet.shrink_to_fit();
				grid[(int)current->pos.x][(int)current->pos.y]->openSet = false;				//Add best choice to closedSet
				closedSet.push_back(current);
				grid[(int)current->pos.x][(int)current->pos.y]->closedSet = true;

				for (int i = 0; i < current->neighbors.size(); i++)								//Check all neighbors to current
				{
					if ((closedSet.empty() || std::find(closedSet.begin(), closedSet.end(), current->neighbors[i]) == closedSet.end())
						&& !current->neighbors[i]->obstacle)		//if currents neighbor isn't in closedSet, update costs
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
			
		}
		//if (!pathFound)
			//std::cout << "No path available" << std::endl;
	}
}

void Pathfinding::AddNeighbors(Node* node)
{
	if (node->pos.x < (cols - 1))							//Add neighbors for node passed in
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
	//Not used atm, still work in progress for optimizing best path
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
			dx::XMFLOAT3 origin;
			dx::XMStoreFloat3(&origin, GetOwner()->GetTransform().GetPosition());
			origin.x = origin.x + x - (cols/2);
			origin.y = origin.y + 7.0f;
			origin.z = origin.z + y - (rows/2);
			Ray ray(origin, DOWN_VEC);
			RayHit hitProps;

			//TERRAIN or default depending on if u can jump from on top of objects
			float distance = 15.0f;
			Physics* phy = Engine::Instance->GetPhysics();
			phy->RaytestSingle(ray, distance, hitProps, FilterGroups::PROPS);

			//this->isGrounded = false;
			//dx::XMFLOAT3 color = dx::XMFLOAT3(1,0,0);
			if (hitProps.object != nullptr) //(hitProps.object != nullptr && hitProps.object->GetName() == "HouseInterior"))// != nullptr )//&& hitProps.object->GetName() == "houseBase"))
			{
				grid[x][y]->obstacle = true;
				//color = dx::XMFLOAT3(0, 0, 1);
			}
			origin.y = origin.y - 15.0f;
			//DShape::DrawBox(ray.origin, dx::XMFLOAT3(0.2, distance, 0.2), color);
			//int randNr = rand() % 100;
			//if(randNr < 30)
			//	grid[x][y]->obstacle = true;
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
		Node* nodeA = correctPath[correctPath.size() - 1];
		Node* nodeB = correctPath[correctPath.size() - 2];

		dx::XMFLOAT3 enemyNextPos;
		dx::XMStoreFloat3(&enemyNextPos, GetOwner()->GetTransform().GetPosition());
		dx::XMFLOAT3 walkTo = { nodeA->pos.x - nodeB->pos.x,0 ,nodeA->pos.y - nodeB->pos.y };

		enemyNextPos = dx::XMFLOAT3(enemyNextPos.x + walkTo.x, 0, enemyNextPos.z + walkTo.z);
		dx::XMVECTOR enemyNextPosVec = dx::XMLoadFloat3(&enemyNextPos);
		dx::XMFLOAT3 vel = enemyRB->GetLinearVelocity();
		dx::XMFLOAT3 move = { -walkTo.x * enemyStatsComp->GetSpeed(), vel.y, -walkTo.z * enemyStatsComp->GetSpeed() };
		enemyRB->SetLinearVelocity(move);

		dx::XMVECTOR dirVec = dx::XMVector3Normalize(dx::XMVectorSubtract(GetOwner()->GetTransform().GetPosition(), enemyNextPosVec));
		dx::XMFLOAT3 dirToNode;
		dx::XMStoreFloat3(&dirToNode, dirVec);
		float angle = 180.f + atan2f(-dirToNode.x, -dirToNode.z) * (180.f / Math::PI);
		float rotation = angle * Math::ToRadians;
		dx::XMVECTOR right = GetOwner()->GetTransform().TransformDirection({ 1,0,0 });
		dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, rotation));
		GetOwner()->GetTransform().SetRotation(eulerRotation);
		enemyRB->SetRotation(eulerRotation);

		dx::XMVECTOR lengthVec = dx::XMVector3Length(dx::XMVectorSubtract(enemyNextPosVec, GetOwner()->GetTransform().GetPosition()));     
		dx::XMStoreFloat(&length, lengthVec);
		if (length <= 0.01)
			correctPath.pop_back();
	}
}