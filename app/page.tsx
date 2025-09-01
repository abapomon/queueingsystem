"use client";

import { useState, useEffect } from "react";

interface QueueData {
  regularQueue: number;
  seniorQueue: number;
}

export default function Home() {
  const [queueData, setQueueData] = useState<QueueData>({
    regularQueue: 0,
    seniorQueue: 0,
  });
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchQueue = async () => {
      try {
        const response = await fetch('/api/queue');
        const data = await response.json();
        setQueueData(data);
      } catch (error) {
        console.error('Error fetching queue data:', error);
      } finally {
        setLoading(false);
      }
    };

    fetchQueue();
    const interval = setInterval(fetchQueue, 1000); // Update every second

    return () => clearInterval(interval);
  }, []);

  if (loading) {
    return (
      <div className="min-h-screen flex items-center justify-center">
        <div className="text-2xl font-semibold">Loading...</div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-gradient-to-br from-blue-50 to-indigo-100 dark:from-gray-900 dark:to-gray-800">
      <div className="container mx-auto px-4 py-8">
        <div className="text-center mb-12">
          <h1 className="text-5xl font-bold text-gray-800 dark:text-white mb-4">
            Queue Management System
          </h1>
          <p className="text-xl text-gray-600 dark:text-gray-300">
            Now Serving
          </p>
        </div>

        <div className="grid md:grid-cols-2 gap-8 max-w-4xl mx-auto">
          {/* Regular Queue */}
          <div className="bg-white dark:bg-gray-800 rounded-2xl shadow-xl p-8 text-center transform hover:scale-105 transition-transform duration-200">
            <div className="mb-6">
              <h2 className="text-2xl font-semibold text-gray-700 dark:text-gray-200 mb-2">
                Regular Queue
              </h2>
              <div className="w-16 h-1 bg-blue-500 mx-auto rounded"></div>
            </div>
            <div className="text-8xl font-bold text-blue-600 dark:text-blue-400 mb-4">
              {queueData.regularQueue.toString().padStart(2, '0')}
            </div>
            <p className="text-gray-500 dark:text-gray-400">
              Current Number
            </p>
          </div>

          {/* Senior/PWD Queue */}
          <div className="bg-white dark:bg-gray-800 rounded-2xl shadow-xl p-8 text-center transform hover:scale-105 transition-transform duration-200">
            <div className="mb-6">
              <h2 className="text-2xl font-semibold text-gray-700 dark:text-gray-200 mb-2">
                Senior / PWD Queue
              </h2>
              <div className="w-16 h-1 bg-green-500 mx-auto rounded"></div>
            </div>
            <div className="text-8xl font-bold text-green-600 dark:text-green-400 mb-4">
              {queueData.seniorQueue.toString().padStart(2, '0')}
            </div>
            <p className="text-gray-500 dark:text-gray-400">
              Current Number
            </p>
          </div>
        </div>

        <div className="text-center mt-12">
          <div className="inline-flex items-center space-x-2 bg-white dark:bg-gray-800 rounded-full px-6 py-3 shadow-lg">
            <div className="w-3 h-3 bg-green-500 rounded-full animate-pulse"></div>
            <span className="text-gray-600 dark:text-gray-300 font-medium">
              Live Updates
            </span>
          </div>
        </div>
      </div>
    </div>
  );
}
