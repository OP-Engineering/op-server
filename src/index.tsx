import OpServer from './NativeOpServer';

export function multiply(a: number, b: number): number {
  return OpServer.multiply(a, b);
}
