using System.Collections;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public SpawnerToggle spawnerToggle;

    public void StartGame()
    {
        spawnerToggle.StartGame();
    }

    public void StopGame()
    {
        spawnerToggle.DisableSpawner();
    }
}
