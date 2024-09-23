export function selectEnvironment<T>(production: T, development: T) {
  if (process.env.NODE_ENV === 'production') {
    return production;
  }
  return development;
}
