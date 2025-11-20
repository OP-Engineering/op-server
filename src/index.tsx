import OpServer from './NativeOpServer';

OpServer.install();

type OPServerProxy = {
  create: () => Object;
};

export interface Request {
  method: string;
  path: string;
  body: string;
  headers: Record<string, string>;
  params: Record<string, string>;
}

export interface Response {
  statusCode: number;
  content: string;
  contentType: string;
  headers: Record<string, string>;
  setHeader(key: string, value: string): void;
}

type RequestHandler = (req: Request, res: Response) => Promise<void>;

declare global {
  var __OPServerProxy: object | undefined;
}

const proxy = global.__OPServerProxy;
export const OPServer = proxy as OPServerProxy;

if (global.__OPServerProxy == null) {
  // Call the synchronous blocking install() function
  const installed = OpServer.install();
  if (!installed) {
    throw new Error(
      `Failed to install op-server: The Native Module could not be installed! Looks like something went wrong when installing JSI bindings, check the native logs for more info`
    );
  }

  // Check again if the constructor now exists. If not, throw an error.
  if (global.__OPServerProxy == null) {
    throw new Error(
      'OP Server native object is not available. Something is wrong. Check the native logs for more information.'
    );
  }
}

export class HttpServer {
  _server: any;
  constructor() {
    this._server = OPServer.create();
  }

  listen(port: number) {
    this._server.listen(port);
  }

  get(path: string, callback: RequestHandler) {
    this._server.callback('GET', path, callback);
  }
  POST(path: string, callback: RequestHandler) {
    this._server.callback('POST', path, callback);
  }
  PUT(path: string, callback: RequestHandler) {
    this._server.callback('PUT', path, callback);
  }
  DELETE(path: string, callback: RequestHandler) {
    this._server.callback('DELETE', path, callback);
  }

  stop() {}
}
