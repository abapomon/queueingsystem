// Simple in-memory queue state
let regularQueue = 0;
let seniorQueue = 0;

export function getQueue() {
  return { regularQueue, seniorQueue };
}

export function incrementRegular() {
  regularQueue++;
  return getQueue();
}

export function incrementSenior() {
  seniorQueue++;
  return getQueue();
}
