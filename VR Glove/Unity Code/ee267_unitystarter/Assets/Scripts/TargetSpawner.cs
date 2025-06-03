using UnityEngine;
using System;


public class TargetSpawner : MonoBehaviour
{
    public event Action<GameObject> OnSpawn;  

    public GameObject prefabToSpawn;
    public float spawnInterval = 2f;
    public float spawnDistance = 2f;
    public float spawnRangeValue = 1f;

    public event Action<GameObject> OneSpawn;

    private float timer = 0f;  
    private bool isSpawning = true;

    void Update()
    {
        if (!isSpawning || prefabToSpawn == null) return;

        timer += Time.deltaTime;
        if (timer >= spawnInterval)
        {
            timer = 0f;
            SpawnTarget();  
        }
    }

    void SpawnTarget()
    {
        Vector3 randomOffset = new Vector3(
            UnityEngine.Random.Range(-spawnRangeValue, spawnRangeValue),
            UnityEngine.Random.Range(-spawnRangeValue, spawnRangeValue),
            0f);

        Vector3 spawnPosition = transform.position + transform.forward * spawnDistance + randomOffset;

        GameObject newTarget = Instantiate(prefabToSpawn, spawnPosition, Quaternion.identity);
        OnSpawn?.Invoke(newTarget);
    }

    public void StartSpawning()
    {
        isSpawning = true;
        timer = 0f;
    }

    public void StopSpawning()
    {
        isSpawning = false;
    }
}
