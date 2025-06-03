using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SpawnerToggle : MonoBehaviour
{
    public GameObject spawnerObject;
    public GameObject[] panelsToRestore;
    public Text scoreText;

    private TargetSpawner spawner;
    private bool isSpawning = false;
    private int score = 0;
    private float gameDuration = 45f;
    private List<GameObject> activeTargets = new List<GameObject>();

    void Start()
    {
        spawner = spawnerObject.GetComponent<TargetSpawner>();
        if (spawnerObject != null)
            spawnerObject.SetActive(false);

        UpdateScoreUI();
    }

    public void StartGame()
    {
        isSpawning = true;
        score = 0;
        UpdateScoreUI();

        foreach (var panel in panelsToRestore)
            panel.SetActive(false);

        EnableSpawner();

        StartCoroutine(GameTimer());
    }

    IEnumerator GameTimer()
    {
        yield return new WaitForSeconds(gameDuration);
        EndGame();
    }

    public void AddScore()
    {
        score++;
        UpdateScoreUI();
    }

    void UpdateScoreUI()
    {
        if (scoreText != null)
            scoreText.text = "Score: " + score;
    }

    void TrackSpawnedTarget(GameObject target)
    {
        activeTargets.Add(target);
    }

    public void EnableSpawner()
    {
        if (spawner != null)
        {
            spawner.OnSpawn += TrackSpawnedTarget;
            spawnerObject.SetActive(true);
        }
    }

    public void DisableSpawner()
    {
        if (spawner != null)
        {
            spawner.OnSpawn -= TrackSpawnedTarget;
            spawnerObject.SetActive(false);
        }
    }

    void EndGame()
    {
        isSpawning = false;

        DisableSpawner();

        foreach (var t in activeTargets)
        {
            if (t != null)
                Destroy(t);
        }

        activeTargets.Clear();

        foreach (var panel in panelsToRestore)
            panel.SetActive(true);
    }


}
