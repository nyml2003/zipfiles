

export function selectEnvironment<T>(production: T, development: T) {
  if (process.env.BASE_ENV === 'prod') {
    return production;
  } else {
    return development;
  }
}

export function isProduction() {
  return process.env.BASE_ENV === 'prod';
}

export function isDevelopment() {
  return process.env.BASE_ENV === 'dev';
}
